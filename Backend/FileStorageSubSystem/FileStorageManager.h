#ifndef FILESTORAGEMANAGER_H
#define FILESTORAGEMANAGER_H

#include "ORM/Repository/FolderRepository.h"
#include "ORM/Repository/FileRepository.h"
#include "ORM/Repository/FileVersionRepository.h"

#include <QJsonObject>

class FileStorageManager
{
private:
    FileStorageManager(const QSqlDatabase &db, const QString &backupFolderPath);

public:
    static const inline QString separator = "/";
    static QSharedPointer<FileStorageManager> instance();

    ~FileStorageManager();

    bool addNewFolder(const QString &symbolFolderPath,
                      const QString &userFolderPath);

    bool addNewFile(const QString &symbolFolderPath,
                    const QString &pathToFile,
                    bool isFrozen = false,
                    const QString &description = "");

    bool appendVersion(const QString &symbolFilePath,
                       const QString &pathToFile,
                       const QString &description = "");

    bool deleteFolder(const QString &symbolFolderPath);
    bool deleteFile(const QString &symbolFilePath);
    bool deleteFileVersion(const QString &symbolFilePath, qlonglong versionNumber);

    bool updateFolderEntity(QJsonObject folderDto, bool updateFrozenStatusOfChildren = false);
    bool updateFileEntity(QJsonObject fileDto);
    bool updateFileVersionEntity(QJsonObject versionDto);

    bool sortFileVersionsInIncreasingOrder(const QString &symbolFilePath);

    QJsonObject getFolderJsonBySymbolPath(const QString &symbolFolderPath, bool includeChildren = false) const;
    QJsonObject getFolderJsonByUserPath(const QString &userFolderPath, bool includeChildren = false) const;
    QJsonObject getFileJsonBySymbolPath(const QString &symbolFilePath, bool includeVersions = false) const;
    QJsonObject getFileJsonByUserPath(const QString &userFilePath, bool includeVersions = false) const;
    QJsonObject getFileVersionJson(const QString &symbolFilePath, qlonglong versionNumber) const;
    QJsonArray getActiveFolderList() const;
    QJsonArray getActiveFileList() const;

    QString getBackupFolderPath() const;
    void setBackupFolderPath(const QString &newBackupFolderPath);

private:
    QString generateRandomFileName();
    QJsonObject folderEntityToJsonObject(const FolderEntity &entity) const;
    QJsonObject fileEntityToJsonObject(const FileEntity &entity) const;
    QJsonObject fileVersionEntityToJsonObject(const FileVersionEntity &entity) const;
    bool sortFileVersionEntities(const FileEntity &parentEntity);

private:
    QString backupFolderPath;
    QSqlDatabase database;
    FolderRepository *folderRepository;
    FileRepository *fileRepository;
    FileVersionRepository *fileVersionRepository;
};

#endif // FILESTORAGEMANAGER_H
