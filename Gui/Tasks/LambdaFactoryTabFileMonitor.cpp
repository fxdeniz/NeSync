﻿#include "LambdaFactoryTabFileMonitor.h"
#include "Backend/FileStorageSubSystem/FileStorageManager.h"

#include <QFileInfo>
#include <QUuid>
#include <QDir>

std::function<bool (QString)> LambdaFactoryTabFileMonitor::isFileExistInDb()
{
    return [](QString pathToFile) -> bool{

        auto fsm = FileStorageManager::instance();
        bool result = fsm->isFileExistByUserFilePath(pathToFile);
        return result;
    };
}

std::function<bool (QString)> LambdaFactoryTabFileMonitor::isFolderExistInDb()
{
    return [](QString pathToFolder) -> bool{

        auto fsm = FileStorageManager::instance();
        bool result = fsm->isFolderExistByUserFolderPath(pathToFolder);
        return result;
    };
}

std::function<bool (QString, QString)> LambdaFactoryTabFileMonitor::isRowExistInModelDb()
{
    return [] (QString connectionName, QString pathOfRowItem) -> bool {

        QString newConnectionName = QUuid::createUuid().toString(QUuid::StringFormat::Id128);
        QSqlDatabase db = QSqlDatabase::cloneDatabase(connectionName, newConnectionName);
        db.open();

        QString resultColumn = "result_column";
        QString queryTemplate = "SELECT COUNT(*) AS %1 FROM %2 WHERE %3 = :3;" ;
        queryTemplate = queryTemplate.arg(resultColumn,                              // 1
                                          TableModelFileMonitor::TABLE_NAME,         // 2
                                          TableModelFileMonitor::COLUMN_NAME_PATH);  // 3

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

std::function<QSqlQuery (QString, QString)> LambdaFactoryTabFileMonitor::fetchFileRowFromModelDb()
{
    return [] (QString connectionName, QString pathToFile) -> QSqlQuery {

        QString newConnectionName = QUuid::createUuid().toString(QUuid::StringFormat::Id128);
        QSqlDatabase db = QSqlDatabase::cloneDatabase(connectionName, newConnectionName);
        db.open();

        QString queryTemplate = "SELECT * FROM %1 WHERE %2 = '%3';" ;
        queryTemplate = queryTemplate.arg(TableModelFileMonitor::TABLE_NAME,        // 1
                                          TableModelFileMonitor::COLUMN_NAME_PATH,  // 2
                                          pathToFile);                              // 3

        QSqlQuery selectQuery(db);
        selectQuery.prepare(queryTemplate);
        selectQuery.exec();

        return selectQuery;
    };
}

std::function<bool (QString, QString)> LambdaFactoryTabFileMonitor::isFileRowReanmedInModelDb()
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

std::function<TableModelFileMonitor::ItemStatus (QString, QString)> LambdaFactoryTabFileMonitor::fetchStatusOfRowFromModelDb()
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

std::function<QString (QString, QString)> LambdaFactoryTabFileMonitor::fetchNameOfRowFromModelDb()
{
    return [](QString connectionName, QString pathToFileOrFolder) -> QString{

        QString newConnectionName = QUuid::createUuid().toString(QUuid::StringFormat::Id128);
        QSqlDatabase db = QSqlDatabase::cloneDatabase(connectionName, newConnectionName);
        db.open();

        QString queryTemplate = "SELECT * FROM %1 WHERE %2 = :3;" ;
        queryTemplate = queryTemplate.arg(TableModelFileMonitor::TABLE_NAME,        // 1
                                          TableModelFileMonitor::COLUMN_NAME_PATH); // 2

        QSqlQuery selectQuery(db);
        selectQuery.prepare(queryTemplate);
        selectQuery.bindValue(":3", pathToFileOrFolder);
        selectQuery.exec();
        selectQuery.next();

        auto record = selectQuery.record();
        auto result = record.value(TableModelFileMonitor::COLUMN_NAME_NAME).toString();

        return result;
    };
}

std::function<QString (QString, QString)> LambdaFactoryTabFileMonitor::fetchOldNameOfRowFromModelDb()
{
    return [](QString connectionName, QString pathToFileOrFolder) -> QString{

        QString newConnectionName = QUuid::createUuid().toString(QUuid::StringFormat::Id128);
        QSqlDatabase db = QSqlDatabase::cloneDatabase(connectionName, newConnectionName);
        db.open();

        QString queryTemplate = "SELECT * FROM %1 WHERE %2 = :3;" ;
        queryTemplate = queryTemplate.arg(TableModelFileMonitor::TABLE_NAME,        // 1
                                          TableModelFileMonitor::COLUMN_NAME_PATH); // 2

        QSqlQuery selectQuery(db);
        selectQuery.prepare(queryTemplate);
        selectQuery.bindValue(":3", pathToFileOrFolder);
        selectQuery.exec();
        selectQuery.next();

        auto record = selectQuery.record();
        auto result = record.value(TableModelFileMonitor::COLUMN_NAME_OLD_NAME).toString();

        return result;
    };
}

std::function<void (QString, QString, TableModelFileMonitor::ItemStatus)> LambdaFactoryTabFileMonitor::insertRowIntoModelDb()
{
    return [](QString connectionName, QString pathOfItem, TableModelFileMonitor::ItemStatus status){

        // Ignore frozen files.
        FileRequestResult fileRecordFromDb = FileStorageManager::instance()->getFileMetaData(pathOfItem);
        if(fileRecordFromDb.isFrozen())
            return;

        // Ignore files when parent user dir is deleted.
        QString parentUserDir = QDir::toNativeSeparators(QFileInfo(pathOfItem).dir().path() + QDir::separator());
        TableModelFileMonitor::ItemStatus parentUserDirStatus = LambdaFactoryTabFileMonitor::fetchStatusOfRowFromModelDb()(connectionName,
                                                                                                                           parentUserDir);
        if(parentUserDirStatus == TableModelFileMonitor::ItemStatus::Deleted)
            return;

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
            parentDir = QDir::toNativeSeparators(dir.path() + QDir::separator());
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

        if(pathOfItem.endsWith(QDir::separator()))
            autoSyncStatus = true;
        else
        {
            if(fileRecordFromDb.isExist() && fileRecordFromDb.isAutoSyncEnabled())
                autoSyncStatus = true;
        }

        insertQuery.bindValue(":7", autoSyncStatus);

        if(status == TableModelFileMonitor::ItemStatus::NewAdded ||
           status == TableModelFileMonitor::ItemStatus::Deleted ||
           status == TableModelFileMonitor::ItemStatus::Missing ||
           autoSyncStatus == false)
        {
            insertQuery.bindValue(":8", TableModelFileMonitor::ProgressStatus::WaitingForUserInteraction);
        }
        else
            insertQuery.bindValue(":8", TableModelFileMonitor::ProgressStatus::ApplyingAutoAction);

        if(!pathOfItem.endsWith(QDir::separator()))
        {
                if(status == TableModelFileMonitor::ItemStatus::NewAdded)
                    insertQuery.bindValue(":9", 1);

                else if(status == TableModelFileMonitor::ItemStatus::Missing)
                    insertQuery.bindValue(":9", fileRecordFromDb.latestVersionNumber());

                else if(status == TableModelFileMonitor::ItemStatus::Modified ||
                        status == TableModelFileMonitor::ItemStatus::MovedAndModified)
                {
                    if(fileRecordFromDb.isExist())
                        insertQuery.bindValue(":9", fileRecordFromDb.latestVersionNumber() + 1);
                }
                else if(status == TableModelFileMonitor::ItemStatus::Moved)
                {
                    if(fileRecordFromDb.isExist())
                        insertQuery.bindValue(":9", fileRecordFromDb.latestVersionNumber());
                }

                else
                {
                    insertQuery.bindValue(":9", 0);
                }
        }

        insertQuery.exec();
    };
}

std::function<void (QString, QString, TableModelFileMonitor::ItemStatus)> LambdaFactoryTabFileMonitor::updateStatusOfRowInModelDb()
{
    return [](QString connectionName, QString pathOfItem, TableModelFileMonitor::ItemStatus status){

        FileRequestResult fileRecordFromDb = FileStorageManager::instance()->getFileMetaData(pathOfItem);
        if(fileRecordFromDb.isFrozen())
            LambdaFactoryTabFileMonitor::deleteRowFromModelDb()(connectionName, pathOfItem);

        QString newConnectionName = QUuid::createUuid().toString(QUuid::StringFormat::Id128);
        QSqlDatabase db = QSqlDatabase::cloneDatabase(connectionName, newConnectionName);
        db.open();

        QString queryTemplate = "UPDATE %1 SET %2 = :2, %3 = :3, %4 = :4, %5 = :5, %6 = :6 WHERE %7 = :7;" ;

        queryTemplate = queryTemplate.arg(TableModelFileMonitor::TABLE_NAME,                    // 1
                                          TableModelFileMonitor::COLUMN_NAME_STATUS,            // 2
                                          TableModelFileMonitor::COLUMN_NAME_TIMESTAMP,         // 3
                                          TableModelFileMonitor::COLUMN_NAME_AUTOSYNC_STATUS,   // 4
                                          TableModelFileMonitor::COLUMN_NAME_PROGRESS,          // 5
                                          TableModelFileMonitor::COLUMN_NAME_CURRENT_VERSION,   // 6
                                          TableModelFileMonitor::COLUMN_NAME_PATH);             // 7

        QSqlQuery updateQuery(db);
        updateQuery.prepare(queryTemplate);

        updateQuery.bindValue(":2", status);
        updateQuery.bindValue(":3", QDateTime::currentDateTime());

        bool autoSyncStatus = false;

        if(pathOfItem.endsWith(QDir::separator()))
            autoSyncStatus = true;
        else
        {
            if(fileRecordFromDb.isExist() && fileRecordFromDb.isAutoSyncEnabled())
                autoSyncStatus = true;
        }

        updateQuery.bindValue(":4", autoSyncStatus);

        if(status == TableModelFileMonitor::ItemStatus::Deleted ||
            status == TableModelFileMonitor::ItemStatus::Missing ||
            autoSyncStatus == false)
        {
            updateQuery.bindValue(":5", TableModelFileMonitor::ProgressStatus::WaitingForUserInteraction);
        }
        else
            updateQuery.bindValue(":5", TableModelFileMonitor::ProgressStatus::ApplyingAutoAction);

        if(!pathOfItem.endsWith(QDir::separator()))
        {
            if(status == TableModelFileMonitor::ItemStatus::NewAdded)
                updateQuery.bindValue(":6", 1);

            else if(status == TableModelFileMonitor::ItemStatus::Missing)
                updateQuery.bindValue(":6", fileRecordFromDb.latestVersionNumber());

            else if(status == TableModelFileMonitor::ItemStatus::Modified ||
                     status == TableModelFileMonitor::ItemStatus::Moved ||
                     status == TableModelFileMonitor::ItemStatus::MovedAndModified)
            {
                if(fileRecordFromDb.isExist())
                    updateQuery.bindValue(":6", fileRecordFromDb.latestVersionNumber() + 1);
            }
            else
            {
                updateQuery.bindValue(":6", 0);
            }
        }

        updateQuery.bindValue(":7", pathOfItem);
        updateQuery.exec();
    };
}

std::function<void (QString, QString, TableModelFileMonitor::ProgressStatus)> LambdaFactoryTabFileMonitor::updateProgressOfRowInModelDb()
{
    return [](QString connectionName, QString pathOfItem, TableModelFileMonitor::ProgressStatus progress){

        QString newConnectionName = QUuid::createUuid().toString(QUuid::StringFormat::Id128);
        QSqlDatabase db = QSqlDatabase::cloneDatabase(connectionName, newConnectionName);
        db.open();

        QString queryTemplate = "UPDATE %1 SET %2 = :2, %3 = :3 WHERE %4 = :4;" ;

        queryTemplate = queryTemplate.arg(TableModelFileMonitor::TABLE_NAME,                // 1
                                          TableModelFileMonitor::COLUMN_NAME_PROGRESS,      // 2
                                          TableModelFileMonitor::COLUMN_NAME_TIMESTAMP,     // 3
                                          TableModelFileMonitor::COLUMN_NAME_PATH);         // 4

        QSqlQuery updateQuery(db);
        updateQuery.prepare(queryTemplate);

        updateQuery.bindValue(":2", progress);
        updateQuery.bindValue(":3", QDateTime::currentDateTime());
        updateQuery.bindValue(":4", pathOfItem);
        updateQuery.exec();
    };
}

std::function<void (QString, QString)> LambdaFactoryTabFileMonitor::deleteRowFromModelDb()
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

std::function<void (QString, QString, QString)> LambdaFactoryTabFileMonitor::updateOldNameOfRowInModelDb()
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

std::function<void (QString, QString, QString)> LambdaFactoryTabFileMonitor::updateNameOfRowInModelDb()
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

std::function<QStringList (QString,
                           TableModelFileMonitor::ItemType,
                           TableModelFileMonitor::ProgressStatus)> LambdaFactoryTabFileMonitor::fetchRowsByProgressFromModelDb()
{
    return [](QString connectionName,
              TableModelFileMonitor::ItemType itemType,
              TableModelFileMonitor::ProgressStatus progressStatus) -> QStringList {

        QStringList result;

        QString newConnectionName = QUuid::createUuid().toString(QUuid::StringFormat::Id128);
        QSqlDatabase db = QSqlDatabase::cloneDatabase(connectionName, newConnectionName);
        db.open();

        QString queryTemplate = "SELECT * FROM %1 WHERE %2 = :2 AND %3 = :3;" ;

        queryTemplate = queryTemplate.arg(TableModelFileMonitor::TABLE_NAME,             // 1
                                          TableModelFileMonitor::COLUMN_NAME_TYPE,       // 2
                                          TableModelFileMonitor::COLUMN_NAME_PROGRESS);  // 3

        QSqlQuery selectQuery(db);
        selectQuery.prepare(queryTemplate);

        selectQuery.bindValue(":2", itemType);
        selectQuery.bindValue(":3", progressStatus);
        selectQuery.exec();

        while(selectQuery.next())
        {
            auto value = selectQuery.record().value(TableModelFileMonitor::ColumnIndex::Path).toString();
            result.append(value);
        }

        return result;
    };
}

std::function<bool (QString, QString)> LambdaFactoryTabFileMonitor::applyActionForFolder()
{
    return [](QString connectionName, QString userFolderPath) -> bool{
        bool result = false;
        auto fsm = FileStorageManager::instance();

        QString newConnectionName = QUuid::createUuid().toString(QUuid::StringFormat::Id128);
        QSqlDatabase db = QSqlDatabase::cloneDatabase(connectionName, newConnectionName);
        db.open();

        QString queryTemplate = "SELECT * FROM %1 WHERE %2 = :2;" ;

        queryTemplate = queryTemplate.arg(TableModelFileMonitor::TABLE_NAME,            // 1
                                          TableModelFileMonitor::COLUMN_NAME_PATH);     // 2

        QSqlQuery selectQuery(db);
        selectQuery.prepare(queryTemplate);

        selectQuery.bindValue(":2", userFolderPath);
        selectQuery.exec();
        selectQuery.next();

        auto record = selectQuery.record();
        auto statusCode = record.value(TableModelFileMonitor::ColumnIndex::Status).value<TableModelFileMonitor::ItemStatus>();

        if(statusCode == TableModelFileMonitor::ItemStatus::NewAdded)
        {
            QString targetSymbolFolderPath = LambdaFactoryTabFileMonitor::generateSymbolFolderPathFromUserDir(userFolderPath);
            result = fsm->addNewFolder(targetSymbolFolderPath);
        }
        else if(statusCode == TableModelFileMonitor::ItemStatus::Moved)
        {
            auto oldName = LambdaFactoryTabFileMonitor::fetchOldNameOfRowFromModelDb()(connectionName, userFolderPath);
            auto dir = QFileInfo(userFolderPath).dir();
            dir.cdUp();
            QString oldUserFolderPath = QDir::toNativeSeparators(dir.path() + QDir::separator() + oldName);
            QString oldSymbolFolderPath = LambdaFactoryTabFileMonitor::generateSymbolFolderPathFromUserDir(oldUserFolderPath);
            QString newFolderName = LambdaFactoryTabFileMonitor::fetchNameOfRowFromModelDb()(connectionName, userFolderPath);
            newFolderName.chop(1);

            result = fsm->renameFolder(oldSymbolFolderPath, newFolderName); // Update FolderRecord
            result = fsm->updateAllUserDirs(oldUserFolderPath, userFolderPath); // Update all matching FileRecords.
        }

        return result;
    };
}

std::function<bool (QString, QString)> LambdaFactoryTabFileMonitor::applyActionForFile()
{
    return [](QString connectionName, QString userFilePath) -> bool{
        bool result = false;
        auto fsm = FileStorageManager::instance();

        QString newConnectionName = QUuid::createUuid().toString(QUuid::StringFormat::Id128);
        QSqlDatabase db = QSqlDatabase::cloneDatabase(connectionName, newConnectionName);
        db.open();

        QString queryTemplate = "SELECT * FROM %1 WHERE %2 = :2;" ;

        queryTemplate = queryTemplate.arg(TableModelFileMonitor::TABLE_NAME,            // 1
                                          TableModelFileMonitor::COLUMN_NAME_PATH);     // 2

        QSqlQuery selectQuery(db);
        selectQuery.prepare(queryTemplate);

        selectQuery.bindValue(":2", userFilePath);
        selectQuery.exec();
        selectQuery.next();

        auto record = selectQuery.record();
        auto statusCode = record.value(TableModelFileMonitor::ColumnIndex::Status).value<TableModelFileMonitor::ItemStatus>();

        if(statusCode == TableModelFileMonitor::ItemStatus::NewAdded)
        {
            QFileInfo info(userFilePath);
            QDir currentUserDir = info.dir();
            QString userDirectory = QDir::toNativeSeparators(info.absolutePath() + QDir::separator());
            QString rootUserDirPath, rootSymbolFolderPath = "";

            while(rootSymbolFolderPath.isEmpty())
            {
                rootUserDirPath = QDir::toNativeSeparators(currentUserDir.path() + QDir::separator());
                rootSymbolFolderPath = fsm->getMatchingSymbolFolderPathForUserDirectory(rootUserDirPath);
                bool isGoneUp = currentUserDir.cdUp();

                if(isGoneUp == false && rootSymbolFolderPath.isEmpty())
                    return false;
            }

            QString targetSymbolFolderPath = rootSymbolFolderPath;
            QStringList childSuffixes = userDirectory.split(rootUserDirPath);
            childSuffixes.removeFirst();

            if(!childSuffixes.first().isEmpty())
            {
                for(QString &token : childSuffixes)
                {
                    token.replace("\\", FileStorageManager::CONST_SYMBOL_DIRECTORY_SEPARATOR);
                    targetSymbolFolderPath.append(token);
                }
            }

            QString note = record.value(TableModelFileMonitor::ColumnIndex::Note).toString();
            result = fsm->addNewFile(userFilePath, targetSymbolFolderPath, false, true, userDirectory, note);
        }
        else if(statusCode == TableModelFileMonitor::ItemStatus::Modified)
        {
            // TODO design FileStorageManager::appendNewVersion() such that
            //      remove call to fsm->getFileMetaData(userFilePath);

            FileRequestResult requestResult = fsm->getFileMetaData(userFilePath);
            result = fsm->appendNewVersion(userFilePath, requestResult.symbolFilePath());
        }
        else if(statusCode == TableModelFileMonitor::ItemStatus::Moved)
        {
            QFileInfo info(userFilePath);
            QString oldUserlFilePath = info.absolutePath() + QDir::separator();
            oldUserlFilePath += record.value(TableModelFileMonitor::ColumnIndex::OldName).toString();
            oldUserlFilePath = QDir::toNativeSeparators(oldUserlFilePath);

            FileRequestResult requestResult = fsm->getFileMetaData(oldUserlFilePath);
            result = fsm->updateNameOfFile(requestResult.symbolFilePath(), info.baseName());

            if(result == true) // If fine name changed succesfully.
            {
                QString userFilePathWithOldExtension = info.absolutePath() + QDir::separator() + info.baseName();
                userFilePathWithOldExtension += requestResult.fileExtension();
                userFilePathWithOldExtension = QDir::toNativeSeparators(userFilePathWithOldExtension);

                requestResult = fsm->getFileMetaData(userFilePathWithOldExtension);
                result = fsm->updateExtensionOfFile(requestResult.symbolFilePath(), "." + info.completeSuffix());
            }
        }
        else if(statusCode == TableModelFileMonitor::ItemStatus::MovedAndModified)
        {
            QFileInfo info(userFilePath);
            QString oldUserlFilePath = info.absolutePath() + QDir::separator();
            oldUserlFilePath += record.value(TableModelFileMonitor::ColumnIndex::OldName).toString();
            oldUserlFilePath = QDir::toNativeSeparators(oldUserlFilePath);

            FileRequestResult requestResult = fsm->getFileMetaData(oldUserlFilePath);
            result = fsm->updateNameOfFile(requestResult.symbolFilePath(), info.baseName());

            if(result == true) // If fine name changed succesfully.
            {
                QString userFilePathWithOldExtension = info.absolutePath() + QDir::separator() + info.baseName();
                userFilePathWithOldExtension += requestResult.fileExtension();
                userFilePathWithOldExtension = QDir::toNativeSeparators(userFilePathWithOldExtension);

                requestResult = fsm->getFileMetaData(userFilePathWithOldExtension);
                result = fsm->updateExtensionOfFile(requestResult.symbolFilePath(), "." + info.completeSuffix());
            }

            if(result == true) // If name and extension changed successfully.
            {
                requestResult = fsm->getFileMetaData(userFilePath);
                result = fsm->appendNewVersion(userFilePath, requestResult.symbolFilePath());
            }
        }
        else if(statusCode == TableModelFileMonitor::ItemStatus::Deleted)
        {
            auto action = record.value(TableModelFileMonitor::ColumnIndex::Action);

            if(action == TableModelFileMonitor::Action::Delete)
            {
                FileRequestResult requestResult = fsm->getFileMetaData(userFilePath);
                result = fsm->deleteFiles({ requestResult.symbolFilePath() });
            }
            else if(action == TableModelFileMonitor::Action::Freeze)
            {
                FileRequestResult requestResult = fsm->getFileMetaData(userFilePath);
                result = fsm->updateFrozenStatusOfFile(requestResult.symbolFilePath(), true);
            }
        }

        return result;
    };
}

QString LambdaFactoryTabFileMonitor::generateSymbolFolderPathFromUserDir(const QString userFolderPath)
{
    QFileInfo info(userFolderPath);
    QDir currentUserDir = info.dir();
    QString rootSymbolFolderPath, rootUserDirPath = "";
    auto fsm = FileStorageManager::instance();

    while(rootSymbolFolderPath.isEmpty())
    {
        rootUserDirPath = QDir::toNativeSeparators(currentUserDir.absolutePath() + QDir::separator());
        rootSymbolFolderPath = fsm->getMatchingSymbolFolderPathForUserDirectory(rootUserDirPath);
        bool isGoneUp = currentUserDir.cdUp();

        // Still couldn't find matching symbol folder path and user directory reached to absolute top.
        if(rootSymbolFolderPath.isEmpty() && isGoneUp == false)
            return "";
    }

    QString userFolderPathSuffix = userFolderPath.split(rootUserDirPath).last();
    userFolderPathSuffix.replace("\\", FileStorageManager::CONST_SYMBOL_DIRECTORY_SEPARATOR);
    QString targetSymbolFolderPath = rootSymbolFolderPath + userFolderPathSuffix;
    return targetSymbolFolderPath;
}
