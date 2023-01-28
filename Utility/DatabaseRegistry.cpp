#include "DatabaseRegistry.h"

#include <QDir>
#include <QUuid>
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>
#include <QRandomGenerator64>

QSqlDatabase DatabaseRegistry::dbFileStorage;
QSqlDatabase DatabaseRegistry::dbFileMonitor;

DatabaseRegistry::DatabaseRegistry()
{

}

QSqlDatabase DatabaseRegistry::fileStorageDatabase()
{
    bool isCreated = dbFileStorage.isValid();

    if(!isCreated)
        createDbFileStorage();

    QString newConnectionName = QUuid::createUuid().toString(QUuid::StringFormat::Id128);

    QSqlDatabase result =  QSqlDatabase::cloneDatabase(dbFileStorage, newConnectionName);

    return result;
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

void DatabaseRegistry::createDbFileStorage()
{
    QString dbPath = QStandardPaths::writableLocation(QStandardPaths::StandardLocation::TempLocation);
    dbPath += QDir::separator();
    dbPath += "backup_2";
    dbPath += QDir::separator();

    QDir().mkdir(dbPath);

    dbPath += "bb_database.db3";

//    dbPath += QUuid::createUuid().toString(QUuid::StringFormat::Id128);
//    dbPath += ".db3";

    dbPath = QDir::toNativeSeparators(dbPath);

    bool isExist = QFile(dbPath).exists();

    dbFileStorage = QSqlDatabase::addDatabase("QSQLITE", "file_storage_db");
    dbFileStorage.setDatabaseName(dbPath);
    dbFileStorage.open();
    dbFileStorage.exec("PRAGMA foreign_keys = ON;");

    if(!isExist)
    {
        QString queryCreateTableFolderEntity;
        queryCreateTableFolderEntity += "CREATE TABLE FolderEntity (";
        queryCreateTableFolderEntity += " symbol_folder_path TEXT NOT NULL"
                                        " UNIQUE GENERATED ALWAYS AS"
                                        " (CASE WHEN parent_folder_path IS NULL"
                                        "       THEN suffix_path"
                                        "       ELSE (parent_folder_path || suffix_path)"
                                        " END)"
                                        " STORED,";
        queryCreateTableFolderEntity += " suffix_path TEXT NOT NULL CHECK (suffix_path != \"\"),";
        queryCreateTableFolderEntity += " parent_folder_path TEXT CHECK (parent_folder_path != \"\"),";
        queryCreateTableFolderEntity += " user_folder_path TEXT UNIQUE CHECK (user_folder_path != \"\"),";
        queryCreateTableFolderEntity += " is_frozen INTEGER NOT NULL DEFAULT 0 CHECK (is_frozen BETWEEN 0 AND 1),";
        queryCreateTableFolderEntity += " FOREIGN KEY (parent_folder_path) REFERENCES FolderEntity (symbol_folder_path)"
                                        " ON DELETE CASCADE ON UPDATE CASCADE,";
        queryCreateTableFolderEntity += " PRIMARY KEY (parent_folder_path, suffix_path)";
        queryCreateTableFolderEntity += ");" ;

        QString queryCreateTableFileEntity;
        queryCreateTableFileEntity += "CREATE TABLE FileEntity (";
        queryCreateTableFileEntity += " symbol_file_path TEXT NOT NULL UNIQUE GENERATED ALWAYS AS (symbol_folder_path || file_name) STORED,";
        queryCreateTableFileEntity += " file_name TEXT NOT NULL CHECK (file_name != \"\"),";
        queryCreateTableFileEntity += " symbol_folder_path TEXT NOT NULL CHECK (symbol_folder_path != \"\"),";
        queryCreateTableFileEntity += "	is_frozen INTEGER NOT NULL DEFAULT 0 CHECK (is_frozen BETWEEN 0 AND 1),";
        queryCreateTableFileEntity += "	FOREIGN KEY (symbol_folder_path) REFERENCES FolderEntity (symbol_folder_path)";
        queryCreateTableFileEntity += " ON DELETE CASCADE ON UPDATE CASCADE,";
        queryCreateTableFileEntity += " PRIMARY KEY (symbol_folder_path, file_name)";
        queryCreateTableFileEntity += ");" ;

        QString queryCreateTableFileVersionEntity;
        queryCreateTableFileVersionEntity += "CREATE TABLE FileVersionEntity (";
        queryCreateTableFileVersionEntity += " symbol_file_path NOT NULL CHECK (symbol_file_path != \"\"),";
        queryCreateTableFileVersionEntity += " version_number INTEGER NOT NULL CHECK (version_number >= 1),";
        queryCreateTableFileVersionEntity += " internal_file_name TEXT NOT NULL UNIQUE CHECK (internal_file_name != \"\"),";
        queryCreateTableFileVersionEntity += " size INTEGER NOT NULL DEFAULT 0 CHECK(size >= 0),";
        queryCreateTableFileVersionEntity += " timestamp TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,";
        queryCreateTableFileVersionEntity += " description TEXT DEFAULT NULL CHECK (description != \"\"),";
        queryCreateTableFileVersionEntity += " hash TEXT DEFAULT NULL CHECK (hash != \"\"),";
        queryCreateTableFileVersionEntity += " FOREIGN KEY (symbol_file_path) REFERENCES FileEntity (symbol_file_path)";
        queryCreateTableFileVersionEntity += " ON DELETE CASCADE ON UPDATE CASCADE,";
        queryCreateTableFileVersionEntity += " PRIMARY KEY (symbol_file_path, version_number)";
        queryCreateTableFileVersionEntity += ");" ;

        dbFileStorage.exec(queryCreateTableFolderEntity);
        dbFileStorage.exec(queryCreateTableFileEntity);
        dbFileStorage.exec(queryCreateTableFileVersionEntity);
        dbFileStorage.exec("INSERT INTO FolderEntity (suffix_path) VALUES('/');");
    }
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
    queryCreateTableFolder += " efsw_id INTEGER DEFAULT NULL CHECK(efsw_id >= 1) UNIQUE,";
    queryCreateTableFolder += " folder_path TEXT NOT NULL,";
    queryCreateTableFolder += " parent_folder_path TEXT,";
    queryCreateTableFolder += " old_folder_name TEXT,";
    queryCreateTableFolder += " status INTEGER NOT NULL DEFAULT 0,";
    queryCreateTableFolder += " event_timestamp TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,";
    queryCreateTableFolder += " PRIMARY KEY(folder_path),";
    queryCreateTableFolder += "	FOREIGN KEY(parent_folder_path) REFERENCES Folder(folder_path) ON DELETE CASCADE ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED";
    queryCreateTableFolder += ");" ;

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
    queryCreateTableFile += " ); " ;

    QString queryCreateTableMonitoringError;
    queryCreateTableMonitoringError += " CREATE TABLE MonitoringError (";
    queryCreateTableMonitoringError += " location TEXT NOT NULL,";
    queryCreateTableMonitoringError += " during TEXT NOT NULL,";
    queryCreateTableMonitoringError += " error_type INTEGER NOT NULL CHECK(error_type BETWEEN -6 AND -1),";
    queryCreateTableMonitoringError += " event_timestamp TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP";
    queryCreateTableMonitoringError += " ); " ;

    dbFileMonitor.exec(queryCreateTableFolder);
    dbFileMonitor.exec(queryCreateTableFile);
    dbFileMonitor.exec(queryCreateTableMonitoringError);

}
