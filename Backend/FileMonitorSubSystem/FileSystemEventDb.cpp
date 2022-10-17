#include "FileSystemEventDb.h"

#include <QDir>
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>
#include <QRandomGenerator>

FileSystemEventDb::FileSystemEventDb()
{
    createDb();
}

void FileSystemEventDb::createDb()
{
    QRandomGenerator *generator = QRandomGenerator::system();
    QString dbPath = QStandardPaths::writableLocation(QStandardPaths::StandardLocation::DesktopLocation);
    dbPath += QDir::separator();
    dbPath += QString::number(generator->generate()) + ".db3";
    dbPath = QDir::toNativeSeparators(dbPath);

    database = QSqlDatabase::addDatabase("QSQLITE", "file_system_event_db");
    database.setDatabaseName(dbPath);
    //database.setDatabaseName(":memory:");

    database.open();
    database.exec("PRAGMA foreign_keys = ON;");

    QString queryCreateTableFolder;
    queryCreateTableFolder += " CREATE TABLE Folder (";
    queryCreateTableFolder += " folder_path TEXT NOT NULL,";
    queryCreateTableFolder += " parent_folder_path TEXT,";
    queryCreateTableFolder += " state INTEGER NOT NULL DEFAULT 0,";
    queryCreateTableFolder += " PRIMARY KEY(folder_path),";
    queryCreateTableFolder += " FOREIGN KEY(parent_folder_path) REFERENCES Folder(folder_path) ON DELETE CASCADE ON UPDATE CASCADE";
    queryCreateTableFolder += ");";

    QString queryCreateTableFile;
    queryCreateTableFile += " CREATE TABLE File (";
    queryCreateTableFile += " file_path TEXT NOT NULL,";
    queryCreateTableFile += " folder_path TEXT,";
    queryCreateTableFile += " state INTEGER NOT NULL DEFAULT 0,";
    queryCreateTableFile += " PRIMARY KEY(file_path),";
    queryCreateTableFile += " FOREIGN KEY(folder_path) REFERENCES Folder(folder_path) ON DELETE CASCADE ON UPDATE CASCADE";
    queryCreateTableFile += " ); ";

    database.exec(queryCreateTableFolder).lastError();
    database.exec(queryCreateTableFile).lastError();
}
