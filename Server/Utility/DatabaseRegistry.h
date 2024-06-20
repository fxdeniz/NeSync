#ifndef DATABASEREGISTRY_H
#define DATABASEREGISTRY_H

#include <QSqlDatabase>

class DatabaseRegistry
{
public:
    DatabaseRegistry();

    static QSqlDatabase fileStorageDatabase();
    static QSqlDatabase fileSystemEventDatabase();

private:
    static void createDbFileStorage();
    static void createDbFileMonitor();
    static QSqlDatabase dbFileStorage;
    static QSqlDatabase dbFileMonitor;
};

#endif // DATABASEREGISTRY_H
