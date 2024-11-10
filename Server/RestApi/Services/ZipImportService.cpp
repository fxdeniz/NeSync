#include "ZipImportService.h"

#include "JsonDtoFormat.h"
#include "FileStorageSubSystem/FileStorageManager.h"

#include <QJsonDocument>
#include <QTemporaryFile>
#include <QOperatingSystemVersion>
#include <quazip/quazip.h>
#include <quazip/quazipfile.h>

ZipImportService::ZipImportService(QObject *parent)
    : QObject{parent}
{}

QString ZipImportService::getZipFilePath() const
{
    return zipFilePath;
}

void ZipImportService::setZipFilePath(QString newZipFilePath)
{
    zipFilePath = newZipFilePath;
}

QJsonArray ZipImportService::getFoldersJson() const
{
    return foldersJson;
}

void ZipImportService::setFoldersJson(const QJsonArray &newFoldersJson)
{
    foldersJson = newFoldersJson;
}

QJsonObject ZipImportService::getFilesJson() const
{
    return filesJson;
}

void ZipImportService::setFilesJson(const QJsonObject &newFoldersJson)
{
    filesJson = newFoldersJson;
}

bool ZipImportService::openArchive() const
{
    QuaZip archive(getZipFilePath());
    bool isOpened = archive.open(QuaZip::Mode::mdUnzip);
    return isOpened;
}

QJsonArray ZipImportService::readFoldersJson()
{
    QuaZip archive(getZipFilePath());

    bool isArchiveOpened = archive.open(QuaZip::mdUnzip);

    if(!isArchiveOpened)
    {
        setFoldersJson({});
        return {};
    }

    QString fileName = "folders.json";
    bool isFileFound = archive.setCurrentFile(fileName);

    if(!isFileFound)
    {
        setFoldersJson({});
        return {};
    }

    QuaZipFile foldersJsonFile(&archive);
    bool isFileOpened = foldersJsonFile.open(QFile::OpenModeFlag::ReadOnly);

    if(!isFileOpened)
    {
        setFoldersJson({});
        return {};
    }

    QJsonParseError parseError;
    QJsonDocument document = QJsonDocument::fromJson(foldersJsonFile.readAll(), &parseError);

    if(parseError.error != QJsonParseError::ParseError::NoError || !document.isArray())
    {
        setFoldersJson({});
        return {};
    }

    // TODO: add json schema validation.

    QJsonArray result = document.array();
    QStringList stringList;

    for (const QVariant &item : result.toVariantList())
        stringList << item.toString();

    std::sort(stringList.begin(), stringList.end(), [](const QString &s1, const QString &s2) {
        return s1.length() < s2.length();
    });

    result = QJsonArray::fromStringList(stringList);

    setFoldersJson(result);
    return result;
}

QJsonObject ZipImportService::readFilesJson()
{
    QuaZip archive(getZipFilePath());

    bool isArchiveOpened = archive.open(QuaZip::mdUnzip);

    if(!isArchiveOpened)
    {
        setFilesJson({});
        return {};
    }

    QString fileName = "files.json";
    bool isFileFound = archive.setCurrentFile(fileName);

    if(!isFileFound)
    {
        setFilesJson({});
        return {};
    }

    QuaZipFile filesJsonFile(&archive);
    bool isFileOpened = filesJsonFile.open(QFile::OpenModeFlag::ReadOnly);

    if(!isFileOpened)
    {
        setFilesJson({});
        return {};
    }

    QJsonParseError parseError;
    QJsonDocument document = QJsonDocument::fromJson(filesJsonFile.readAll(), &parseError);

    if(parseError.error != QJsonParseError::ParseError::NoError || !document.isObject())
    {
        setFilesJson({});
        return {};
    }

    // TODO: add json schema validation.

    QJsonObject result = document.object();
    setFilesJson(result);
    return result;
}

bool ZipImportService::importFileVersion(QString symbolFilePath, qulonglong versionNumber)
{
    QuaZip archive(getZipFilePath());
    bool isArchiveOpened = archive.open(QuaZip::Mode::mdUnzip);

    if(!isArchiveOpened)
        return false;

    auto fsm = FileStorageManager::instance();

    QJsonObject file = getFilesJson()[symbolFilePath].toObject();
    QJsonObject version;

    // TODO: Find something more efficent than traversing and searching the version number.
    for(const QJsonValue &value : file[JsonKeys::File::VersionList].toArray())
    {
        QJsonObject current = value.toObject();
        qulonglong number = value[JsonKeys::FileVersion::VersionNumber].toInteger();

        if(versionNumber == number)
        {
            version = current;
            break;
        }
    }

    archive.setCurrentFile(version[JsonKeys::FileVersion::InternalFileName].toString());

    QuaZipFile fileInZip(&archive);
    bool isSourceOpened = fileInZip.open(QFile::OpenModeFlag::ReadOnly);
    QTemporaryFile tempFile;
    bool isTempOpened = tempFile.open();

    if(!isSourceOpened || !isTempOpened)
        return false;

    while(!fileInZip.atEnd())
    {
        qlonglong bytesWritten = tempFile.write(fileInZip.read(104857600)); // Read up to 100mb.
        tempFile.flush();

        if(bytesWritten <= -1)
            return false;
    }

    bool result = false;

    // Below temp.fileName() calls returns the absolute path with file name.
    if(versionNumber != 1) // If adding versions other than first version.
        result = fsm->appendVersion(symbolFilePath, tempFile.fileName(), version[JsonKeys::FileVersion::Description].toString());
    else
    {
        result = fsm->addNewFile(file[JsonKeys::File::SymbolFolderPath].toString(),
                                 tempFile.fileName(),
                                 true,
                                 file[JsonKeys::File::FileName].toString(),
                                 version[JsonKeys::FileVersion::Description].toString());

    }

    return result;
}
