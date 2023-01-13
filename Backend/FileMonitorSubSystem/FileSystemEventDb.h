#ifndef FILESYSTEMEVENTDB_H
#define FILESYSTEMEVENTDB_H

#include <QSqlDatabase>


class FileSystemEventDb
{
public:
    enum ItemStatus
    {
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
    bool setStatusOfFile(const QString pathToFile, ItemStatus status);
    bool setEfswIDofFolder(const QString &pathToFolder, long id);

private:
    QSqlDatabase database;

    void createDb();
};

#endif // FILESYSTEMEVENTDB_H
