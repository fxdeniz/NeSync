#include "ZipImportService.h"

#include <QJsonDocument>
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
