#include "DatabaseRegistry.h"

#include <QDir>
#include <QUuid>
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>
#include <QRandomGenerator64>

QSqlDatabase DatabaseRegistry::dbFileMonitor;

DatabaseRegistry::DatabaseRegistry()
{

}

QSqlDatabase DatabaseRegistry::fileSystemEventDatabase()
{
    bool isCreated = dbFileMonitor.isValid();

    if(!isCreated)
        createDbFileMonitor();

    QString newConnectionName = QUuid::createUuid().toString(QUuid::StringFormat::Id128);

    QSqlDatabase result =  QSqlDatabase::cloneDatabase(dbFileMonitor, newConnectionName);

    return result;
}

void DatabaseRegistry::createDbFileMonitor()
{
    QRandomGenerator *generator = QRandomGenerator::system();
    //QRandomGenerator *generator = new QRandomGenerator();
    QString dbPath = QStandardPaths::writableLocation(QStandardPaths::StandardLocation::DesktopLocation);
    dbPath += QDir::separator();
    dbPath += QString::number(generator->generate()) + ".db3";
    dbPath = QDir::toNativeSeparators(dbPath);

    dbFileMonitor = QSqlDatabase::addDatabase("QSQLITE", "file_system_event_db");
    dbFileMonitor.setConnectOptions("QSQLITE_OPEN_URI;QSQLITE_ENABLE_SHARED_CACHE");

    QString connectionString = "file:%1?mode=memory&cache=shared";
    QString randomDbFileName = QUuid::createUuid().toString(QUuid::StringFormat::Id128) + ".db3";
    connectionString = connectionString.arg(randomDbFileName);

    dbFileMonitor.setDatabaseName(connectionString);
    //dbFileMonitor.setDatabaseName(dbPath);
    //database.setDatabaseName(":memory:");

    dbFileMonitor.open();
    dbFileMonitor.exec("PRAGMA foreign_keys = ON;");

    QString queryCreateTableFolder;
    queryCreateTableFolder += " CREATE TABLE Folder (";
    queryCreateTableFolder += " efsw_id INTEGER CHECK(efsw_id >= 1) UNIQUE,";
    queryCreateTableFolder += " folder_path TEXT NOT NULL,";
    queryCreateTableFolder += " parent_folder_path TEXT,";
    queryCreateTableFolder += " old_folder_name TEXT,";
    queryCreateTableFolder += " status INTEGER NOT NULL DEFAULT 0,";
    queryCreateTableFolder += " event_timestamp TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,";
    queryCreateTableFolder += " PRIMARY KEY(folder_path),";
    queryCreateTableFolder += "	FOREIGN KEY(parent_folder_path) REFERENCES Folder(folder_path) ON DELETE CASCADE ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED";
    queryCreateTableFolder += ");";

    QString queryCreateTableFile;
    queryCreateTableFile += " CREATE TABLE File (";
    queryCreateTableFile += " file_path TEXT NOT NULL UNIQUE GENERATED ALWAYS AS (folder_path || file_name) STORED,";
    queryCreateTableFile += " folder_path TEXT NOT NULL,";
    queryCreateTableFile += " file_name TEXT NOT NULL,";
    queryCreateTableFile += " old_file_name TEXT,";
    queryCreateTableFile += " status INTEGER NOT NULL DEFAULT 0 CHECK(status BETWEEN 0 AND 5),";
    queryCreateTableFile += " event_timestamp TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,";
    queryCreateTableFile += " PRIMARY KEY(folder_path, file_name),";
    queryCreateTableFile += " FOREIGN KEY(folder_path) REFERENCES Folder(folder_path) ON DELETE CASCADE ON UPDATE CASCADE";
    queryCreateTableFile += " ); ";

    QString queryCreateTableMonitoringError;
    queryCreateTableMonitoringError += " CREATE TABLE MonitoringError (";
    queryCreateTableMonitoringError += " location TEXT NOT NULL,";
    queryCreateTableMonitoringError += " during TEXT NOT NULL,";
    queryCreateTableMonitoringError += " error_type INTEGER NOT NULL CHECK(error_type BETWEEN -6 AND -1),";
    queryCreateTableMonitoringError += " event_timestamp TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP";
    queryCreateTableMonitoringError += " ); ";

    qDebug() << "create folder table query has error = " << dbFileMonitor.exec(queryCreateTableFolder).lastError();
    qDebug() << "create file table query has error = " << dbFileMonitor.exec(queryCreateTableFile).lastError();
    qDebug() << "create monitroing error table query has error = " << dbFileMonitor.exec(queryCreateTableMonitoringError).lastError();

}
