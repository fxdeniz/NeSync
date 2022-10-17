#ifndef FILESYSTEMEVENTDB_H
#define FILESYSTEMEVENTDB_H

#include <QSqlDatabase>

class FileSystemEventDb
{
public:
    FileSystemEventDb();

    bool addFolder(const QString &pathToFolder);
    bool addFile(const QString &pathToFile);

private:
    QSqlDatabase database;

    void createDb();
};

#endif // FILESYSTEMEVENTDB_H
