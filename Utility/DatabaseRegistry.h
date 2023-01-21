#ifndef DATABASEREGISTRY_H
#define DATABASEREGISTRY_H

#include <QSqlDatabase>

class DatabaseRegistry
{
public:
    DatabaseRegistry();

    static QSqlDatabase inMemoryFileSystemEventDatabase();
    static QString fileSystemEventDbFolderTableName();
    static QString fileSystemEventDbFileTableName();


private:
    static void createDbFileMonitor();
    static QSqlDatabase dbFileMonitor;
};

#endif // DATABASEREGISTRY_H
