#include "ZipExportService.h"

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

}

bool ZipExportService::addFoldersJson()
{

}

bool ZipExportService::addFileJson()
{

}

bool ZipExportService::addFileToZip(QString symbolFilePath, qlonglong versionNumber)
{

}

