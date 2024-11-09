#include "ZipExportService.h"

#include "JsonDtoFormat.h"
#include "FileStorageSubSystem/FileStorageManager.h"

#include <QJsonDocument>
#include <QOperatingSystemVersion>
#include <quazip/quazip.h>
#include <quazip/quazipfile.h>


ZipExportService::ZipExportService(QObject *parent)
    : QObject{parent}
{}

QString ZipExportService::getZipFilePath() const
{
    return zipFilePath;
}

void ZipExportService::setZipFilePath(const QString &newZipFilePath)
{
    zipFilePath = newZipFilePath;
}

QString ZipExportService::getRootSymbolFolderPath() const
{
    return rootSymbolFolderPath;
}

void ZipExportService::setRootSymbolFolderPath(const QString &newRootSymbolFolderPath)
{
    rootSymbolFolderPath = newRootSymbolFolderPath;
}

QJsonObject ZipExportService::getFilesJson() const
{
    return filesJson;
}

void ZipExportService::setFilesJson(const QJsonObject &newFilesJson)
{
    filesJson = newFilesJson;
}

bool ZipExportService::createArchive()
{
    QuaZip archive(getZipFilePath());
    bool isCreated = archive.open(QuaZip::Mode::mdCreate);
    return isCreated;
}

bool ZipExportService::addFoldersJson()
{
    auto fsm = FileStorageManager::instance();

    QList<QJsonObject> folderStack;
    folderStack.append(fsm->getFolderJsonBySymbolPath(getRootSymbolFolderPath()));

    QStringList folderJsonContent;

    while(!folderStack.isEmpty())
    {
        QJsonObject currentFolder = folderStack.first();
        folderStack.removeFirst();

        folderJsonContent.append(currentFolder[JsonKeys::Folder::SymbolFolderPath].toString());

        currentFolder = fsm->getFolderJsonBySymbolPath(currentFolder[JsonKeys::Folder::SymbolFolderPath].toString(), true);

        for(const QJsonValue &value : currentFolder[JsonKeys::Folder::ChildFolders].toArray())
            folderStack.append(value.toObject());
    }

    QuaZip archive(getZipFilePath());
    bool isArchiveOpened = archive.open(QuaZip::Mode::mdAdd);

    QuaZipFile foldersJsonFile(&archive);
    bool isFileOpened = foldersJsonFile.open(QFile::OpenModeFlag::WriteOnly, QuaZipNewInfo("folders.json"));

    if(!isArchiveOpened || !isFileOpened)
        return false;

    std::sort(folderJsonContent.begin(), folderJsonContent.end(), [](const QString &s1, const QString &s2) {
        return s1.length() < s2.length();
    });

    QJsonDocument document(QJsonArray::fromStringList(folderJsonContent));
    qint64 bytesWritten = foldersJsonFile.write(document.toJson(QJsonDocument::JsonFormat::Indented));

    if(bytesWritten <= -1)
        return false;

    return true;
}

bool ZipExportService::addFileJson()
{
    auto fsm = FileStorageManager::instance();

    QList<QJsonObject> folderStack;
    folderStack.append(fsm->getFolderJsonBySymbolPath(getRootSymbolFolderPath()));

    QJsonObject filesJsonContent;

    while(!folderStack.isEmpty())
    {
        QJsonObject currentFolder = folderStack.first();
        folderStack.removeFirst();

        currentFolder = fsm->getFolderJsonBySymbolPath(currentFolder[JsonKeys::Folder::SymbolFolderPath].toString(), true);

        for(const QJsonValue &value : currentFolder[JsonKeys::Folder::ChildFolders].toArray())
            folderStack.append(value.toObject());

        for(const QJsonValue &value : currentFolder[JsonKeys::Folder::ChildFiles].toArray())
        {
            QJsonObject currentFile = fsm->getFileJsonBySymbolPath(value.toObject()[JsonKeys::File::SymbolFilePath].toString(), true);
            currentFile.remove(JsonKeys::IsExist);
            currentFile.remove(JsonKeys::File::IsFrozen);
            currentFile.remove(JsonKeys::File::UserFilePath);

            QJsonArray versionList = currentFile[JsonKeys::File::VersionList].toArray();

            for (qlonglong index = 0; index < versionList.size(); ++index)
            {
                QJsonObject version = versionList[index].toObject();
                version.remove(JsonKeys::IsExist);
                version.remove(JsonKeys::FileVersion::NewVersionNumber);
                versionList[index] = version;
            }

            currentFile[JsonKeys::File::VersionList] = versionList;

            filesJsonContent.insert(currentFile[JsonKeys::File::SymbolFilePath].toString(), currentFile);
        }
    }

    QuaZip archive(getZipFilePath());
    bool isArchiveOpened = archive.open(QuaZip::Mode::mdAdd);

    QuaZipFile filesJsonFile(&archive);
    bool isFileOpened = filesJsonFile.open(QFile::OpenModeFlag::WriteOnly, QuaZipNewInfo("files.json"));

    if(!isArchiveOpened || !isFileOpened)
    {
        setFilesJson({});
        return false;
    }

    QJsonDocument document(filesJsonContent);
    qint64 bytesWritten = filesJsonFile.write(document.toJson(QJsonDocument::JsonFormat::Indented));

    setFilesJson(filesJsonContent);

    if(bytesWritten <= -1)
    {
        setFilesJson({});
        return false;
    }

    return true;
}

bool ZipExportService::addFileToZip(QString symbolFilePath, qlonglong versionNumber)
{   
    QJsonObject file = getFilesJson()[symbolFilePath].toObject();
    QJsonObject version;

    // TODO: Find something more efficent than traversing and searching the version number.
    for(const QJsonValue &value : file[JsonKeys::File::VersionList].toArray())
    {
        QJsonObject current = value.toObject();
        qlonglong number = value[JsonKeys::FileVersion::VersionNumber].toInteger();

        if(versionNumber == number)
        {
            version = current;
            break;
        }
    }

    QuaZip archive(this->zipFilePath);
    bool isArchiveOpened = archive.open(QuaZip::Mode::mdAdd);

    if(!isArchiveOpened)
        return false;

    auto fsm = FileStorageManager::instance();

    QString internalFileName = version[JsonKeys::FileVersion::InternalFileName].toString();
    QString internalFilePath = fsm->getStorageFolderPath() + internalFileName;

    QFile rawFile(internalFilePath);
    bool isReadable = rawFile.open(QFile::OpenModeFlag::ReadOnly);

    if(!isReadable)
        return false;

    QuaZipNewInfo info(internalFileName, internalFilePath);
    QuaZipFile fileInZip(&archive);
    fileInZip.open(QFile::OpenModeFlag::WriteOnly, info);

    while(!rawFile.atEnd())
    {
        // Write up to 100mb in every iteration.
        qlonglong bytesWritten = fileInZip.write(rawFile.read(104857600));
        if(bytesWritten == -1)
            return false;
    }

    return true;
}

