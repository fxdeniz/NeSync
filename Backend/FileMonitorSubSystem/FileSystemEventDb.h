#ifndef FILESYSTEMEVENTDB_H
#define FILESYSTEMEVENTDB_H

#include "efsw/efsw.hpp"
#include "qdatetime.h"
#include <QSqlDatabase>


class FileSystemEventDb
{
public:
    enum ItemStatus
    {
        Invalid = -1,
        Undefined = 0,
        NewAdded = 1,
        Updated = 2,
        Renamed = 3,
        UpdatedAndRenamed = 4,
        Deleted = 5
    };

    FileSystemEventDb();

    bool isFolderExist(const QString &pathToFolder) const;
    bool isFileExist(const QString &pathToFile) const;
    bool addFolder(const QString &pathToFolder);
    bool addFile(const QString &pathToFile);
    bool deleteFolder(const QString &pathToFolder);
    bool deleteFile(const QString &pathToFile);
    bool setStatusOfFolder(const QString &pathToFolder, ItemStatus status);
    bool setStatusOfFile(const QString &pathToFile, ItemStatus status);
    bool setNameOfFile(const QString &pathToFile, const QString &newName);
    bool setOldNameOfFile(const QString &pathToFile, const QString &oldName);
    bool setEfswIDofFolder(const QString &pathToFolder, long id);
    efsw::WatchID getEfswIDofFolder(const QString &pathToFolder) const;
    ItemStatus getStatusOfFile(const QString &pathToFile) const;

private:
    QSqlDatabase database;

    QSqlRecord getFileRow(const QString &pathToFile) const;
    void createDb();
};

#endif // FILESYSTEMEVENTDB_H
