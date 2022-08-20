#include "LambdaFactoryTabFileMonitor.h"
#include "Backend/FileStorageSubSystem/FileStorageManager.h"

#include <QFileInfo>
#include <QUuid>
#include <QDir>

std::function<bool (QString)> LambdaFactoryTabFileMonitor::lambdaIsFileExistInDb()
{
    return [](QString pathToFile) -> bool{

        auto fsm = FileStorageManager::instance();
        bool result = fsm->isFileExistByUserFilePath(pathToFile);
        return result;
    };
}

std::function<bool (QString)> LambdaFactoryTabFileMonitor::lambdaIsFolderExistInDb()
{
    return [](QString pathToFolder) -> bool{

        auto fsm = FileStorageManager::instance();
        bool result = fsm->isFolderExistByUserFolderPath(pathToFolder);
        return result;
    };
}

std::function<bool (QString, QString)> LambdaFactoryTabFileMonitor::lambdaIsRowExistInModelDb()
{
    return [] (QString connectionName, QString pathOfRowItem) -> bool {

        QString newConnectionName = QUuid::createUuid().toString(QUuid::StringFormat::Id128);
        QSqlDatabase db = QSqlDatabase::cloneDatabase(connectionName, newConnectionName);
        db.open();

        QString resultColumn = "result_column";
        QString queryTemplate = "SELECT COUNT(*) AS %1 FROM %2 WHERE %3 = :3;" ;
        queryTemplate = queryTemplate.arg(resultColumn,                               // 1
                                          TableModelFileMonitor::TABLE_NAME,        // 2
                                          TableModelFileMonitor::COLUMN_NAME_PATH); // 3

        QSqlQuery selectQuery(db);
        selectQuery.prepare(queryTemplate);
        selectQuery.bindValue(":3", pathOfRowItem);
        selectQuery.exec();
        selectQuery.next();

        auto count = selectQuery.record().value(resultColumn).toInt();
        bool result = false;

        if(count > 0)
            result = true;

        return result;
    };
}

std::function<QSqlQuery (QString, QString)> LambdaFactoryTabFileMonitor::lambdaFetchFileRowFromModelDb()
{
    return [] (QString connectionName, QString pathToFile) -> QSqlQuery {

        QString newConnectionName = QUuid::createUuid().toString(QUuid::StringFormat::Id128);
        QSqlDatabase db = QSqlDatabase::cloneDatabase(connectionName, newConnectionName);
        db.open();

        QString queryTemplate = "SELECT * FROM %1 WHERE %2 = '%3';" ;
        queryTemplate = queryTemplate.arg(TableModelFileMonitor::TABLE_NAME,       // 1
                                          TableModelFileMonitor::COLUMN_NAME_PATH, // 2
                                          pathToFile);                               // 3

        QSqlQuery selectQuery(db);
        selectQuery.prepare(queryTemplate);
        selectQuery.exec();

        return selectQuery;
    };
}

std::function<bool (QString, QString)> LambdaFactoryTabFileMonitor::lambdaIsFileRowReanmedInModelDb()
{
    return [](QString connectionName, QString pathToFile) -> bool{

        bool result = true;
        QString newConnectionName = QUuid::createUuid().toString(QUuid::StringFormat::Id128);
        QSqlDatabase db = QSqlDatabase::cloneDatabase(connectionName, newConnectionName);
        db.open();

        QString queryTemplate = "SELECT * FROM %1 WHERE %2 = :3;" ;
        queryTemplate = queryTemplate.arg(TableModelFileMonitor::TABLE_NAME,        // 1
                                          TableModelFileMonitor::COLUMN_NAME_PATH); // 2

        QSqlQuery selectQuery(db);
        selectQuery.prepare(queryTemplate);
        selectQuery.bindValue(":3", pathToFile);
        selectQuery.exec();
        selectQuery.next();

        auto value = selectQuery.record().value(TableModelFileMonitor::COLUMN_NAME_OLD_NAME).toString();

        if(value.isEmpty() || value.isNull())
            result = false;

        return result;
    };
}

std::function<TableModelFileMonitor::ItemStatus (QString, QString)> LambdaFactoryTabFileMonitor::lambdaFetchStatusOfRowFromModelDb()
{
    return [](QString connectionName, QString pathToFile) -> TableModelFileMonitor::ItemStatus{

        QString newConnectionName = QUuid::createUuid().toString(QUuid::StringFormat::Id128);
        QSqlDatabase db = QSqlDatabase::cloneDatabase(connectionName, newConnectionName);
        db.open();

        QString queryTemplate = "SELECT * FROM %1 WHERE %2 = :3;" ;
        queryTemplate = queryTemplate.arg(TableModelFileMonitor::TABLE_NAME,        // 1
                                          TableModelFileMonitor::COLUMN_NAME_PATH); // 2

        QSqlQuery selectQuery(db);
        selectQuery.prepare(queryTemplate);
        selectQuery.bindValue(":3", pathToFile);
        selectQuery.exec();
        selectQuery.next();

        auto record = selectQuery.record();
        auto result = record.value(TableModelFileMonitor::COLUMN_NAME_STATUS).value<TableModelFileMonitor::ItemStatus>();

        return result;
    };
}

std::function<void (QString, QString, TableModelFileMonitor::ItemStatus)> LambdaFactoryTabFileMonitor::lambdaInsertRowIntoModelDb()
{
    return [](QString connectionName, QString pathOfItem, TableModelFileMonitor::ItemStatus status){

        QString newConnectionName = QUuid::createUuid().toString(QUuid::StringFormat::Id128);
        QSqlDatabase db = QSqlDatabase::cloneDatabase(connectionName, newConnectionName);
        db.open();

        QString queryTemplate = "INSERT INTO %1 (%2, %3, %4, %5, %6, %7, %8, %9) "
                                "VALUES(:2, :3, :4, :5, :6, :7, :8, :9);" ;

        queryTemplate = queryTemplate.arg(TableModelFileMonitor::TABLE_NAME,                    // 1
                                          TableModelFileMonitor::COLUMN_NAME_NAME,              // 2
                                          TableModelFileMonitor::COLUMN_NAME_PARENT_DIR,        // 3
                                          TableModelFileMonitor::COLUMN_NAME_TYPE,              // 4
                                          TableModelFileMonitor::COLUMN_NAME_STATUS,            // 5
                                          TableModelFileMonitor::COLUMN_NAME_TIMESTAMP,         // 6
                                          TableModelFileMonitor::COLUMN_NAME_AUTOSYNC_STATUS,   // 7
                                          TableModelFileMonitor::COLUMN_NAME_PROGRESS,          // 8
                                          TableModelFileMonitor::COLUMN_NAME_CURRENT_VERSION);  // 9

        QSqlQuery insertQuery(db);
        insertQuery.prepare(queryTemplate);

        QFileInfo info(pathOfItem);

        if(pathOfItem.endsWith(QDir::separator()))
            insertQuery.bindValue(":2", info.dir().dirName() + QDir::separator());
        else
            insertQuery.bindValue(":2", info.fileName());

        QString parentDir;
        if(pathOfItem.endsWith(QDir::separator()))
        {
            QDir dir = info.dir();
            dir.cdUp();
            parentDir = QDir::toNativeSeparators(dir.absolutePath()) + QDir::separator();
        }
        else
            parentDir = QDir::toNativeSeparators(info.absolutePath()) + QDir::separator();

        insertQuery.bindValue(":3", parentDir);

        if(pathOfItem.endsWith(QDir::separator()))
            insertQuery.bindValue(":4", TableModelFileMonitor::ItemType::Folder);
        else // If we're inserting a file
            insertQuery.bindValue(":4", TableModelFileMonitor::ItemType::File);

        insertQuery.bindValue(":5", status);
        insertQuery.bindValue(":6", QDateTime::currentDateTime());

        bool autoSyncStatus = false;
        FileRequestResult fileRecordFromDb = FileStorageManager::instance()->getFileMetaData(pathOfItem);

        if(pathOfItem.endsWith(QDir::separator()))
            autoSyncStatus = true;
        else
        {
            if(fileRecordFromDb.isExist() && fileRecordFromDb.isAutoSyncEnabled())
                autoSyncStatus = true;
        }

        insertQuery.bindValue(":7", autoSyncStatus);

        if(status == TableModelFileMonitor::ItemStatus::Deleted || autoSyncStatus == false)
            insertQuery.bindValue(":8", TableModelFileMonitor::ProgressStatus::WaitingForUserInteraction);
        else
            insertQuery.bindValue(":8", TableModelFileMonitor::ProgressStatus::ApplyingAutoAction);

        if(!pathOfItem.endsWith(QDir::separator()))
        {
            if(fileRecordFromDb.isExist())
                insertQuery.bindValue(":9", fileRecordFromDb.latestVersionNumber() + 1);
            else
                insertQuery.bindValue(":9", 1);
        }

        insertQuery.exec();
    };
}

std::function<void (QString, QString, TableModelFileMonitor::ItemStatus)> LambdaFactoryTabFileMonitor::lambdaUpdateStatusOfRowInModelDb()
{
    return [](QString connectionName, QString pathToItem, TableModelFileMonitor::ItemStatus status){

        QString newConnectionName = QUuid::createUuid().toString(QUuid::StringFormat::Id128);
        QSqlDatabase db = QSqlDatabase::cloneDatabase(connectionName, newConnectionName);
        db.open();

        QString queryTemplate = "UPDATE %1 SET %2 = :2, %3 = :3 WHERE %4 = :4;" ;

        queryTemplate = queryTemplate.arg(TableModelFileMonitor::TABLE_NAME,            // 1
                                          TableModelFileMonitor::COLUMN_NAME_STATUS,    // 2
                                          TableModelFileMonitor::COLUMN_NAME_TIMESTAMP, // 3
                                          TableModelFileMonitor::COLUMN_NAME_PATH);     // 4

        QSqlQuery updateQuery(db);
        updateQuery.prepare(queryTemplate);

        updateQuery.bindValue(":2", status);
        updateQuery.bindValue(":3", QDateTime::currentDateTime());
        updateQuery.bindValue(":4", pathToItem);
        updateQuery.exec();
    };
}

std::function<void (QString, QString)> LambdaFactoryTabFileMonitor::lambdaDeleteRowFromModelDb()
{
    return [](QString connectionName, QString pathToFile){

        QString newConnectionName = QUuid::createUuid().toString(QUuid::StringFormat::Id128);
        QSqlDatabase db = QSqlDatabase::cloneDatabase(connectionName, newConnectionName);
        db.open();

        QString queryTemplate = "DELETE FROM %1 WHERE %2 = :2;" ;

        queryTemplate = queryTemplate.arg(TableModelFileMonitor::TABLE_NAME,        // 1
                                          TableModelFileMonitor::COLUMN_NAME_PATH); // 2

        QSqlQuery deleteQuery(db);
        deleteQuery.prepare(queryTemplate);

        deleteQuery.bindValue(":2", pathToFile);
        deleteQuery.exec();
    };
}

std::function<void (QString, QString, QString)> LambdaFactoryTabFileMonitor::lambdaUpdateOldNameOfRowInModelDb()
{
    return [](QString connectionName, QString pathToFile, QString oldName){

        QString newConnectionName = QUuid::createUuid().toString(QUuid::StringFormat::Id128);
        QSqlDatabase db = QSqlDatabase::cloneDatabase(connectionName, newConnectionName);
        db.open();

        QString queryTemplate = "UPDATE %1 SET %2 = :2 WHERE %3 = :3;" ;

        queryTemplate = queryTemplate.arg(TableModelFileMonitor::TABLE_NAME,           // 1
                                          TableModelFileMonitor::COLUMN_NAME_OLD_NAME, // 2
                                          TableModelFileMonitor::COLUMN_NAME_PATH);    // 3

        QSqlQuery updateQuery(db);
        updateQuery.prepare(queryTemplate);

        updateQuery.bindValue(":2", oldName);
        updateQuery.bindValue(":3", pathToFile);
        updateQuery.exec();
    };
}

std::function<void (QString, QString, QString)> LambdaFactoryTabFileMonitor::lambdaUpdateNameOfRowInModelDb()
{
    return [](QString connectionName, QString pathToFile, QString oldName){

        QString newConnectionName = QUuid::createUuid().toString(QUuid::StringFormat::Id128);
        QSqlDatabase db = QSqlDatabase::cloneDatabase(connectionName, newConnectionName);
        db.open();

        QString queryTemplate = "UPDATE %1 SET %2 = :2 WHERE %3 = :3;" ;

        queryTemplate = queryTemplate.arg(TableModelFileMonitor::TABLE_NAME,        // 1
                                          TableModelFileMonitor::COLUMN_NAME_NAME,  // 2
                                          TableModelFileMonitor::COLUMN_NAME_PATH); // 3

        QSqlQuery updateQuery(db);
        updateQuery.prepare(queryTemplate);

        updateQuery.bindValue(":2", oldName);
        updateQuery.bindValue(":3", pathToFile);
        updateQuery.exec();
    };
}
