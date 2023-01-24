#ifndef DATABASEREGISTRY_H
#define DATABASEREGISTRY_H

#include <QSqlDatabase>

class DatabaseRegistry
{
public:
    DatabaseRegistry();

    static QSqlDatabase fileSystemEventDatabase();

private:
    static void createDbFileMonitor();
    static QSqlDatabase dbFileMonitor;
};

#endif // DATABASEREGISTRY_H
