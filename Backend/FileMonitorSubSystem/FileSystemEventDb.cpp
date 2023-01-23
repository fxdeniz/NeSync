#include "FileSystemEventDb.h"

#include <QDir>
#include <QUuid>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QStandardPaths>
#include <QRandomGenerator>

FileSystemEventDb::FileSystemEventDb(const QSqlDatabase &eventDb)
{
    database = eventDb;
    database.open();
}

FileSystemEventDb::~FileSystemEventDb()
{
    database.close();
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

    if(database.transaction())
    {
        QString nativePath = QDir::toNativeSeparators(pathToFolder);

        if(nativePath.endsWith(QDir::separator()))
            nativePath.chop(1);

        QString queryTemplate = "INSERT INTO Folder(folder_path, parent_folder_path) VALUES(:1, :2);" ;
        QStringList folderNames = nativePath.split(QDir::separator());
        QString parentFolderPath = "";

        for(const QString &folder : folderNames)
        {
            // Start constructing from root path
            QString currentSubFolderPath = folder + QDir::separator();
            QString currentFolderPath = parentFolderPath + currentSubFolderPath;

            bool isInsertingRootPath = currentFolderPath == QDir::separator();

            bool isAlreadyInDb = isFolderExist(currentFolderPath);

            if(!isAlreadyInDb) // If root path not in db
            {
                QSqlQuery query(database);
                query.prepare(queryTemplate);

                query.bindValue(":1", currentFolderPath);

                if(isInsertingRootPath)
                    query.bindValue(":2", QVariant()); // Bind null value.
                else
                    query.bindValue(":2", parentFolderPath);

                query.exec();

                if(query.lastError().type() != QSqlError::ErrorType::NoError)
                {
                    database.rollback();
                    return false;
                }
            }

            // Set last inserted path as root path
            parentFolderPath += currentSubFolderPath;
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
        QString queryTemplate = "INSERT INTO File(folder_path, file_name) VALUES(:1, :2);" ;
        QSqlQuery query(database);
        query.prepare(queryTemplate);

        query.bindValue(":1", nativeFolderPath);
        query.bindValue(":2", info.fileName());

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

bool FileSystemEventDb::setStatusOfFolder(const QString &pathToFolder, ItemStatus status)
{
    bool result = false;

    QString nativePath = QDir::toNativeSeparators(pathToFolder);

    if(!nativePath.endsWith(QDir::separator()))
        nativePath.append(QDir::separator());

    bool isFolderInDb = isFolderExist(nativePath);

    if(isFolderInDb)
    {
        QString queryTemplate = "UPDATE Folder SET status = :1, event_timestamp = :2 WHERE folder_path = :3;" ;
        QSqlQuery query(database);
        query.prepare(queryTemplate);

        query.bindValue(":1", status);
        query.bindValue(":2", QDateTime::currentDateTime());
        query.bindValue(":3", nativePath);

        query.exec();

        if(query.lastError().type() == QSqlError::ErrorType::NoError)
            result = true;
    }

    return result;
}

bool FileSystemEventDb::setStatusOfFile(const QString &pathToFile, ItemStatus status)
{
    bool result = false;

    QString nativePath = QDir::toNativeSeparators(pathToFile);

    bool isFileInDb = isFileExist(nativePath);

    if(isFileInDb)
    {
        QString queryTemplate = "UPDATE File SET status = :1, event_timestamp = :2 WHERE file_path = :3;" ;
        QSqlQuery query(database);
        query.prepare(queryTemplate);

        query.bindValue(":1", status);
        query.bindValue(":2", QDateTime::currentDateTime());
        query.bindValue(":3", nativePath);

        query.exec();

        if(query.lastError().type() == QSqlError::ErrorType::NoError)
            result = true;
    }

    return result;
}

bool FileSystemEventDb::setPathOfFolder(const QString &pathToFolder, const QString &newPath)
{
    bool result = false;

    QString oldNativePath = QDir::toNativeSeparators(pathToFolder);
    QString newNativePath = QDir::toNativeSeparators(newPath);

    if(!oldNativePath.endsWith(QDir::separator()))
        oldNativePath.append(QDir::separator());

    if(!newNativePath.endsWith(QDir::separator()))
        newNativePath.append(QDir::separator());

    bool isFolderInDb = isFolderExist(oldNativePath);

    if(isFolderInDb)
    {
        QString queryTemplate = "UPDATE Folder SET folder_path = :1 WHERE folder_path = :2;" ;
        QSqlQuery query(database);
        query.prepare(queryTemplate);

        query.bindValue(":1", newNativePath);
        query.bindValue(":2", oldNativePath);

        query.exec();

        if(query.lastError().type() == QSqlError::ErrorType::NoError)
            result = true;
    }

    return result;
}

bool FileSystemEventDb::setNameOfFile(const QString &pathToFile, const QString &newName)
{
    bool result = false;

    QString nativePath = QDir::toNativeSeparators(pathToFile);

    bool isFileInDb = isFileExist(nativePath);

    if(isFileInDb)
    {
        QString queryTemplate = "UPDATE File SET file_name = :1 WHERE file_path = :2;" ;
        QSqlQuery query(database);
        query.prepare(queryTemplate);

        query.bindValue(":1", newName);
        query.bindValue(":2", nativePath);

        query.exec();

        if(query.lastError().type() == QSqlError::ErrorType::NoError)
            result = true;
    }

    return result;
}

bool FileSystemEventDb::setOldNameOfFolder(const QString &pathToFolder, const QString &oldName)
{
    bool result = false;

    QString nativePath = QDir::toNativeSeparators(pathToFolder);

    if(!nativePath.endsWith(QDir::separator()))
        nativePath.append(QDir::separator());

    bool isFolderInDb = isFolderExist(nativePath);

    if(isFolderInDb)
    {
        QString queryTemplate = "UPDATE Folder SET old_folder_name = :1 WHERE folder_path = :2;" ;
        QSqlQuery query(database);
        query.prepare(queryTemplate);

        query.bindValue(":1", oldName);
        query.bindValue(":2", nativePath);

        query.exec();

        if(query.lastError().type() == QSqlError::ErrorType::NoError)
            result = true;
    }

    return result;
}

bool FileSystemEventDb::setOldNameOfFile(const QString &pathToFile, const QString &oldName)
{
    bool result = false;

    QString nativePath = QDir::toNativeSeparators(pathToFile);

    bool isFileInDb = isFileExist(nativePath);

    if(isFileInDb)
    {
        QString queryTemplate = "UPDATE File SET old_file_name = :1 WHERE file_path = :2;" ;
        QSqlQuery query(database);
        query.prepare(queryTemplate);

        query.bindValue(":1", oldName);
        query.bindValue(":2", nativePath);

        query.exec();

        if(query.lastError().type() == QSqlError::ErrorType::NoError)
            result = true;
    }

    return result;
}

bool FileSystemEventDb::setEfswIDofFolder(const QString &pathToFolder, long id)
{
    bool result = false;

    bool isFolderInDb = isFolderExist(pathToFolder);

    if(!isFolderInDb)
        return false;

    QString nativePath = QDir::toNativeSeparators(pathToFolder);

    if(!nativePath.endsWith(QDir::separator()))
        nativePath.append(QDir::separator());

    QString queryTemplate = "UPDATE Folder SET efsw_id = :1 WHERE folder_path = :2;" ;

    QSqlQuery query(database);
    query.prepare(queryTemplate);

    if(id > 0)
        query.bindValue(":1", (qlonglong) id);
    else
        query.bindValue(":1", QVariant()); // set efsw_id as NULL

    query.bindValue(":2", nativePath);

    query.exec();

    if(query.lastError().type() == QSqlError::ErrorType::NoError)
        result = true;

    return result;
}

efsw::WatchID FileSystemEventDb::getEfswIDofFolder(const QString &pathToFolder) const
{
    efsw::WatchID result = -1;
    QString nativePath = QDir::toNativeSeparators(pathToFolder);

    if(!nativePath.endsWith(QDir::separator()))
        nativePath.append(QDir::separator());

    bool isFolderInDb = isFolderExist(nativePath);

    if(isFolderInDb)
    {
        QSqlRecord record = getFolderRow(nativePath);
        result = record.value("efsw_id").value<efsw::WatchID>();
    }

    return result;
}

FileSystemEventDb::ItemStatus FileSystemEventDb::getStatusOfFolder(const QString &pathToFolder) const
{
    FileSystemEventDb::ItemStatus result = FileSystemEventDb::ItemStatus::Invalid;
    QString nativePath = QDir::toNativeSeparators(pathToFolder);

    if(!nativePath.endsWith(QDir::separator()))
        nativePath.append(QDir::separator());

    bool isFolderInDb = isFolderExist(nativePath);

    if(isFolderInDb)
    {
        QSqlRecord row = getFolderRow(nativePath);
        result = row.value("status").value<FileSystemEventDb::ItemStatus>();
    }

    return result;
}

FileSystemEventDb::ItemStatus FileSystemEventDb::getStatusOfFile(const QString &pathToFile) const
{
    FileSystemEventDb::ItemStatus result = FileSystemEventDb::ItemStatus::Invalid;
    QString nativePath = QDir::toNativeSeparators(pathToFile);

    bool isFileInDb = isFileExist(nativePath);

    if(isFileInDb)
    {
        QSqlRecord row = getFileRow(nativePath);
        result = row.value("status").value<FileSystemEventDb::ItemStatus>();
    }

    return result;
}

QSqlRecord FileSystemEventDb::getFolderRow(const QString &pathToFolder) const
{
    QString queryTemplate = "SELECT * FROM Folder WHERE folder_path = :1;" ;

    QSqlQuery query(database);
    query.prepare(queryTemplate);

    query.bindValue(":1", pathToFolder);
    query.exec();
    query.next();

    return query.record();
}

QSqlRecord FileSystemEventDb::getFileRow(const QString &pathToFile) const
{
    QString queryTemplate = "SELECT * FROM File WHERE file_path = :1;" ;

    QSqlQuery query(database);
    query.prepare(queryTemplate);

    query.bindValue(":1", pathToFile);
    query.exec();
    query.next();

    return query.record();
}