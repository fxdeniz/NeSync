#ifndef V2_FILESTORAGEMANAGER_H
#define V2_FILESTORAGEMANAGER_H

#include "ORM/Repository/FolderRepository.h"
#include "ORM/Repository/FileRepository.h"
#include "ORM/Repository/FileVersionRepository.h"

#include <QJsonObject>

class V2_FileStorageManager
{
private:
    V2_FileStorageManager(const QSqlDatabase &db, const QString &backupFolderPath);

public:
    static const inline QString separator = "/";
    static QSharedPointer<V2_FileStorageManager> instance();

    ~V2_FileStorageManager();

    bool addNewFolder(const QString &symbolFolderPath,
                      const QString &userFolderPath);

    bool addNewFile(const QString &symbolFolderPath,
                    const QString &pathToFile,
                    bool isFrozen = false,
                    const QString &description = "");

    bool appendVersion(const QString &symbolFilePath,
                       const QString &pathToFile,
                       const QString &description = "");

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

private:
    QString backupFolderPath;
    QSqlDatabase database;
    FolderRepository *folderRepository;
    FileRepository *fileRepository;
    FileVersionRepository *fileVersionRepository;
};

#endif // V2_FILESTORAGEMANAGER_H
