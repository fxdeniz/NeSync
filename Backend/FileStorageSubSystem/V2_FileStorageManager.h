#ifndef V2_FILESTORAGEMANAGER_H
#define V2_FILESTORAGEMANAGER_H

#include "ORM/Repository/FolderRepository.h"
#include "ORM/Repository/FileRepository.h"
#include "ORM/Repository/FileVersionRepository.h"

class V2_FileStorageManager
{
public:
    static const inline QString separator = "/";

    V2_FileStorageManager(const QSqlDatabase &db, const QString &backupFolderPath);
    ~V2_FileStorageManager();

    bool addNewFolder(const QString &parentSymbolFolderPath,
                      const QString &suffixSymbolFolderPath,
                      const QString &userFolderPath);

    bool addNewFile(const QString &symbolFolderPath,
                    const QString &pathToFile,
                    const QString &description = "",
                    bool isFrozen = false);

    QString getBackupFolderPath() const;
    void setBackupFolderPath(const QString &newBackupFolderPath);

private:
    QString generateRandomFileName();

    QString backupFolderPath;
    QSqlDatabase database;
    FolderRepository *folderRepository;
    FileRepository *fileRepository;
    FileVersionRepository *fileVersionRepository;
};

#endif // V2_FILESTORAGEMANAGER_H
