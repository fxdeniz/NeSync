#include "LambdaFactoryTabFileMonitor.h"
#include "Backend/FileStorageSubSystem/FileStorageManager.h"

#include <QFileInfo>
#include <QUuid>
#include <QDir>

std::function<bool (QString)> LambdaFactoryTabFileMonitor::lambdaIsFileExistInDb()
{
    return [](QString pathToFile) -> bool{

        auto fsm = FileStorageManager::instance();
        bool isFileExistInDb = fsm->isFileExistByUserFilePath(pathToFile);
        return isFileExistInDb;
    };
}

std::function<bool (QString, QString)> LambdaFactoryTabFileMonitor::lambdaIsFileRowExistInModelDb()
{
    return [] (QString connectionName, QString pathToFile) -> bool {

        QString newConnectionName = QUuid::createUuid().toString(QUuid::StringFormat::Id128);
        QSqlDatabase db = QSqlDatabase::cloneDatabase(connectionName, newConnectionName);
        db.open();

        QString resultColumn = "result_column";
        QString queryTemplate = "SELECT COUNT(*) AS %1 FROM %2 WHERE %3 = '%4';" ;
        queryTemplate = queryTemplate.arg(resultColumn,                              // 1
                                          V2TableModelFileMonitor::TABLE_NAME,       // 2
                                          V2TableModelFileMonitor::COLUMN_NAME_PATH, // 3
                                          pathToFile);                               // 4

        QSqlQuery selectQuery(db);
        selectQuery.prepare(queryTemplate);
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
        queryTemplate = queryTemplate.arg(V2TableModelFileMonitor::TABLE_NAME,       // 1
                                          V2TableModelFileMonitor::COLUMN_NAME_PATH, // 2
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
        queryTemplate = queryTemplate.arg(V2TableModelFileMonitor::TABLE_NAME,        // 1
                                          V2TableModelFileMonitor::COLUMN_NAME_PATH); // 2

        QSqlQuery selectQuery(db);
        selectQuery.prepare(queryTemplate);
        selectQuery.bindValue(":3", pathToFile);
        selectQuery.exec();
        selectQuery.next();

        auto value = selectQuery.record().value(V2TableModelFileMonitor::COLUMN_NAME_OLD_NAME).toString();

        if(value.isEmpty() || value.isNull())
            result = false;

        return result;
    };
}

std::function<V2TableModelFileMonitor::TableItemStatus (QString, QString)> LambdaFactoryTabFileMonitor::lambdaFetchStatusOfFileRowFromModelDb()
{
    return [](QString connectionName, QString pathToFile) -> V2TableModelFileMonitor::TableItemStatus{

        QString newConnectionName = QUuid::createUuid().toString(QUuid::StringFormat::Id128);
        QSqlDatabase db = QSqlDatabase::cloneDatabase(connectionName, newConnectionName);
        db.open();

        QString queryTemplate = "SELECT * FROM %1 WHERE %2 = :3;" ;
        queryTemplate = queryTemplate.arg(V2TableModelFileMonitor::TABLE_NAME,        // 1
                                          V2TableModelFileMonitor::COLUMN_NAME_PATH); // 2

        QSqlQuery selectQuery(db);
        selectQuery.prepare(queryTemplate);
        selectQuery.bindValue(":3", pathToFile);
        selectQuery.exec();
        selectQuery.next();

        auto record = selectQuery.record();
        auto result = record.value(V2TableModelFileMonitor::COLUMN_NAME_STATUS).value<V2TableModelFileMonitor::TableItemStatus>();

        return result;
    };
}

std::function<void (QString, QString, V2TableModelFileMonitor::TableItemStatus)> LambdaFactoryTabFileMonitor::lambdaInsertFileRowIntoModelDb()
{
    return [](QString connectionName, QString pathToFile, V2TableModelFileMonitor::TableItemStatus status){

        QString newConnectionName = QUuid::createUuid().toString(QUuid::StringFormat::Id128);
        QSqlDatabase db = QSqlDatabase::cloneDatabase(connectionName, newConnectionName);
        db.open();

        QString queryTemplate = "INSERT INTO %1 (%2, %3, %4, %5, %6) "
                                "VALUES(:2, :3, :4, :5, :6);" ;

        queryTemplate = queryTemplate.arg(V2TableModelFileMonitor::TABLE_NAME,              // 1
                                          V2TableModelFileMonitor::COLUMN_NAME_NAME,        // 2
                                          V2TableModelFileMonitor::COLUMN_NAME_PARENT_DIR,  // 3
                                          V2TableModelFileMonitor::COLUMN_NAME_TYPE,        // 4
                                          V2TableModelFileMonitor::COLUMN_NAME_STATUS,      // 5
                                          V2TableModelFileMonitor::COLUMN_NAME_TIMESTAMP);  // 6

        QSqlQuery insertQuery(db);
        insertQuery.prepare(queryTemplate);

        QFileInfo info(pathToFile);
        auto parentDir = QDir::toNativeSeparators(info.absolutePath()) + QDir::separator();

        insertQuery.bindValue(":2", info.fileName());
        insertQuery.bindValue(":3", parentDir);
        insertQuery.bindValue(":4", V2TableModelFileMonitor::TableItemType::File);
        insertQuery.bindValue(":5", status);
        insertQuery.bindValue(":6", QDateTime::currentDateTime());
        insertQuery.exec();
    };
}

std::function<void (QString, QString, V2TableModelFileMonitor::TableItemStatus)> LambdaFactoryTabFileMonitor::lambdaUpdateStatusOfFileRowInModelDb()
{
    return [](QString connectionName, QString pathToFile, V2TableModelFileMonitor::TableItemStatus status){

        QString newConnectionName = QUuid::createUuid().toString(QUuid::StringFormat::Id128);
        QSqlDatabase db = QSqlDatabase::cloneDatabase(connectionName, newConnectionName);
        db.open();

        QString queryTemplate = "UPDATE %1 SET %2 = :2, %3 = :3 WHERE %4 = :4;" ;

        queryTemplate = queryTemplate.arg(V2TableModelFileMonitor::TABLE_NAME,            // 1
                                          V2TableModelFileMonitor::COLUMN_NAME_STATUS,    // 2
                                          V2TableModelFileMonitor::COLUMN_NAME_TIMESTAMP, // 3
                                          V2TableModelFileMonitor::COLUMN_NAME_PATH);     // 4

        QSqlQuery updateQuery(db);
        updateQuery.prepare(queryTemplate);

        updateQuery.bindValue(":2", status);
        updateQuery.bindValue(":3", QDateTime::currentDateTime());
        updateQuery.bindValue(":4", pathToFile);
        updateQuery.exec();
    };
}

std::function<void (QString, QString)> LambdaFactoryTabFileMonitor::lambdaDeleteFileRowFromModelDb()
{
    return [](QString connectionName, QString pathToFile){

        QString newConnectionName = QUuid::createUuid().toString(QUuid::StringFormat::Id128);
        QSqlDatabase db = QSqlDatabase::cloneDatabase(connectionName, newConnectionName);
        db.open();

        QString queryTemplate = "DELETE FROM %1 WHERE %2 = :2;" ;

        queryTemplate = queryTemplate.arg(V2TableModelFileMonitor::TABLE_NAME,        // 1
                                          V2TableModelFileMonitor::COLUMN_NAME_PATH); // 2

        QSqlQuery deleteQuery(db);
        deleteQuery.prepare(queryTemplate);

        deleteQuery.bindValue(":2", pathToFile);
        deleteQuery.exec();
    };
}

std::function<void (QString, QString, QString)> LambdaFactoryTabFileMonitor::lambdaUpdateOldNameOfFileRowInModelDb()
{
    return [](QString connectionName, QString pathToFile, QString oldName){

        QString newConnectionName = QUuid::createUuid().toString(QUuid::StringFormat::Id128);
        QSqlDatabase db = QSqlDatabase::cloneDatabase(connectionName, newConnectionName);
        db.open();

        QString queryTemplate = "UPDATE %1 SET %2 = :2 WHERE %3 = :3;" ;

        queryTemplate = queryTemplate.arg(V2TableModelFileMonitor::TABLE_NAME,           // 1
                                          V2TableModelFileMonitor::COLUMN_NAME_OLD_NAME, // 2
                                          V2TableModelFileMonitor::COLUMN_NAME_PATH);    // 3

        QSqlQuery updateQuery(db);
        updateQuery.prepare(queryTemplate);

        updateQuery.bindValue(":2", oldName);
        updateQuery.bindValue(":3", pathToFile);
        updateQuery.exec();
    };
}

std::function<void (QString, QString, QString)> LambdaFactoryTabFileMonitor::lambdaUpdateNameOfFileRowInModelDb()
{
    return [](QString connectionName, QString pathToFile, QString oldName){

        QString newConnectionName = QUuid::createUuid().toString(QUuid::StringFormat::Id128);
        QSqlDatabase db = QSqlDatabase::cloneDatabase(connectionName, newConnectionName);
        db.open();

        QString queryTemplate = "UPDATE %1 SET %2 = :2 WHERE %3 = :3;" ;

        queryTemplate = queryTemplate.arg(V2TableModelFileMonitor::TABLE_NAME,        // 1
                                          V2TableModelFileMonitor::COLUMN_NAME_NAME,  // 2
                                          V2TableModelFileMonitor::COLUMN_NAME_PATH); // 3

        QSqlQuery updateQuery(db);
        updateQuery.prepare(queryTemplate);

        updateQuery.bindValue(":2", oldName);
        updateQuery.bindValue(":3", pathToFile);
        updateQuery.exec();
    };
}
