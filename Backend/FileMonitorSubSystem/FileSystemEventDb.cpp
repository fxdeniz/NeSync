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

bool FileSystemEventDb::isFolderExist(const QString &pathToFolder) const
{
    bool result = false;

    QString nativePath = QDir::toNativeSeparators(pathToFolder);

    if(!nativePath.endsWith(QDir::separator()))
        nativePath.append(QDir::separator());

    QString queryTemplate = "SELECT * FROM Folder WHERE folder_path = :1;" ;

    QSqlQuery query(database);
    query.prepare(queryTemplate);
    query.bindValue(":1", nativePath);
    query.exec();

    if(query.next())
        result = true;

    return result;
}

bool FileSystemEventDb::isFileExist(const QString &pathToFile) const
{
    bool result = false;

    QString nativePath = QDir::toNativeSeparators(pathToFile);

    QString queryTemplate = "SELECT * FROM File WHERE file_path = :1;" ;

    QSqlQuery query(database);
    query.prepare(queryTemplate);
    query.bindValue(":1", nativePath);
    query.exec();

    if(query.next())
        result = true;

    return result;
}

bool FileSystemEventDb::addFolder(const QString &pathToFolder)
{
    bool result = false;

    QDir dir(pathToFolder);
    bool isDirExist = dir.exists();

    if(!isDirExist)
        return false;

    // Insert from bottom to up (including pathToFolder and root dir)
    if(database.transaction())
    {
        bool isGoneUp = true;

        while(isGoneUp)
        {
            QString nativePath = QDir::toNativeSeparators(dir.absolutePath());

            if(!nativePath.endsWith(QDir::separator()))
                nativePath.append(QDir::separator());

            // If current folder exist in db (therefore all upper level folder also exist in Db),
            //      then goto commit line
            bool isAlreadyInDb = isFolderExist(nativePath);
            if(isAlreadyInDb)
                break;

            QDir parentDir = dir;
            parentDir.cdUp();

            QString nativeParentPath = QDir::toNativeSeparators(parentDir.absolutePath());

            if(!nativeParentPath.endsWith(QDir::separator()))
                nativeParentPath.append(QDir::separator());

            isGoneUp = dir.cdUp();

            QString queryTemplate;

            if(isGoneUp)
                queryTemplate = "INSERT INTO Folder(folder_path, parent_folder_path) VALUES(:1, :2);" ;
            else
                queryTemplate = "INSERT INTO Folder(folder_path) VALUES(:1);" ;

            QSqlQuery query(database);
            query.prepare(queryTemplate);

            query.bindValue(":1", nativePath);
            if(isGoneUp)
                query.bindValue(":2", nativeParentPath);

            query.exec();

            if(query.lastError().type() != QSqlError::ErrorType::NoError)
            {
                database.rollback();
                return false;
            }
        }

        result = database.commit();
    }

    return result;
}

bool FileSystemEventDb::addFile(const QString &pathToFile)
{
    bool result = false;
    QFileInfo info(pathToFile);

    if(!info.exists())
        return false;

    if(!info.isFile())
        return false;

    bool isAlreadyInDb = isFileExist(pathToFile);

    if(isAlreadyInDb)
        return true;

    QString nativeFolderPath = QDir::toNativeSeparators(info.absolutePath());

    if(!nativeFolderPath.endsWith(QDir::separator()))
        nativeFolderPath.append(QDir::separator());

    bool isFolderAdded = addFolder(nativeFolderPath);

    if(isFolderAdded)
    {
        QString nativePath = QDir::toNativeSeparators(pathToFile);

        QString queryTemplate = "INSERT INTO File(file_path, folder_path) VALUES(:1, :2);" ;
        QSqlQuery query(database);
        query.prepare(queryTemplate);

        query.bindValue(":1", nativePath);
        query.bindValue(":2", nativeFolderPath);

        query.exec();

        if(query.lastError().type() == QSqlError::ErrorType::NoError)
            result = true;
    }

    return result;
}

bool FileSystemEventDb::deleteFolder(const QString &pathToFolder)
{
    bool result = false;

    QString nativePath = QDir::toNativeSeparators(pathToFolder);

    if(!nativePath.endsWith(QDir::separator()))
        nativePath.append(QDir::separator());

    bool isFolderInDb = isFolderExist(nativePath);

    if(isFolderInDb)
    {
        QString queryTemplate = "DELETE FROM Folder WHERE folder_path = :1;" ;
        QSqlQuery query(database);
        query.prepare(queryTemplate);

        query.bindValue(":1", nativePath);

        query.exec();

        if(query.lastError().type() == QSqlError::ErrorType::NoError)
            result = true;
    }

    return result;
}

bool FileSystemEventDb::deleteFile(const QString &pathToFile)
{
    bool result = false;

    QString nativePath = QDir::toNativeSeparators(pathToFile);

    bool isFileInDb = isFileExist(nativePath);

    if(isFileInDb)
    {
        QString queryTemplate = "DELETE FROM File WHERE file_path = :1;" ;
        QSqlQuery query(database);
        query.prepare(queryTemplate);

        query.bindValue(":1", nativePath);

        query.exec();

        if(query.lastError().type() == QSqlError::ErrorType::NoError)
            result = true;
    }

    return result;
}

void FileSystemEventDb::createDb()
{
    QRandomGenerator *generator = QRandomGenerator::system();
    //QRandomGenerator *generator = new QRandomGenerator();
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
    queryCreateTableFolder += " timestamp TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,";
    queryCreateTableFolder += " PRIMARY KEY(folder_path),";
    queryCreateTableFolder += "	FOREIGN KEY(parent_folder_path) REFERENCES Folder(folder_path) ON DELETE CASCADE ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED";
    queryCreateTableFolder += ");";

    QString queryCreateTableFile;
    queryCreateTableFile += " CREATE TABLE File (";
    queryCreateTableFile += " file_path TEXT NOT NULL,";
    queryCreateTableFile += " folder_path TEXT,";
    queryCreateTableFile += " state INTEGER NOT NULL DEFAULT 0,";
    queryCreateTableFile += " timestamp TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,";
    queryCreateTableFile += " PRIMARY KEY(file_path),";
    queryCreateTableFile += " FOREIGN KEY(folder_path) REFERENCES Folder(folder_path) ON DELETE CASCADE ON UPDATE CASCADE";
    queryCreateTableFile += " ); ";

    database.exec(queryCreateTableFolder).lastError();
    database.exec(queryCreateTableFile).lastError();
}
