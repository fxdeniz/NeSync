#ifndef FILESYSTEMEVENTDB_H
#define FILESYSTEMEVENTDB_H

#include <QSqlDatabase>


class FileSystemEventDb
{
public:
    FileSystemEventDb();

    bool isFolderExist(const QString &pathToFolder) const;
    bool isFileExist(const QString &pathToFile) const;
    bool addFolder(const QString &pathToFolder);
    bool addFile(const QString &pathToFile);
    bool deleteFolder(const QString &pathToFolder);
    bool deleteFile(const QString &pathToFile);
    bool setEfswIDforFolder(const QString &pathToFolder, long id);

private:
    QSqlDatabase database;

    void createDb();
};

#endif // FILESYSTEMEVENTDB_H
