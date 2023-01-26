#ifndef V2_FILESTORAGEMANAGER_H
#define V2_FILESTORAGEMANAGER_H

#include "ORM/Repository/FolderRepository.h"

class V2_FileStorageManager
{
public:
    static const inline QString separator = "/";

    V2_FileStorageManager(const QSqlDatabase &db);
    ~V2_FileStorageManager();

    bool addNewFolder(const QString &parentSymbolFolderPath,
                      const QString &suffixSymbolFolderPath,
                      const QString &userFolderPath);


private:
    QSqlDatabase database;
    FolderRepository *folderRepository;
};

#endif // V2_FILESTORAGEMANAGER_H
