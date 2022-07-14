#include "RowInserter.h"

#include <QSqlQuery>

#include "QuerySaveGroupItem.h"
#include "QueryFolderRecord.h"
#include "QueryFileVersion.h"
#include "QueryFileRecord.h"
#include "QueryFileEvent.h"

RowInserter::RowInserter(const QSqlDatabase &db) : BaseSqlPrimitive(db, "")
{
}

PtrTo_RowFolderRecord RowInserter::insertRootFolder(const QString &rootDir)
{
    QSqlQuery query(this->getDb());
    QString queryTemplate = "INSERT INTO %1 (%2, %3) "
                            "VALUES(NULL, :3);" ;

    queryTemplate = queryTemplate.arg(TABLE_NAME_FOLDER_RECORD,                       // 1
                                      TABLE_FOLDER_RECORD_COLNAME_PARENT_DIRECTORY,   // 2
                                      TABLE_FOLDER_RECORD_COLNAME_SUFFIX_DIRECTORY);  // 3

    query.prepare(queryTemplate);
    query.bindValue(":3", rootDir);
    query.exec();

    this->setLastError(query.lastError());

    QueryFolderRecord queries(this->getDb());
    PtrTo_RowFolderRecord result = queries.selectRowByDirectory(rootDir);

    return result;
}

PtrTo_RowFolderRecord RowInserter::insertChildFolder(const QString &parentDir, const QString &suffixDir)
{
    QSqlQuery query(this->getDb());
    QString queryTemplate = "INSERT INTO %1 (%2, %3) "
                            "VALUES(:2, :3);" ;

    queryTemplate = queryTemplate.arg(TABLE_NAME_FOLDER_RECORD,                       // 1
                                      TABLE_FOLDER_RECORD_COLNAME_PARENT_DIRECTORY,   // 2
                                      TABLE_FOLDER_RECORD_COLNAME_SUFFIX_DIRECTORY);  // 3

    query.prepare(queryTemplate);
    query.bindValue(":2", parentDir);
    query.bindValue(":3", suffixDir);
    query.exec();

    this->setLastError(query.lastError());

    QueryFolderRecord queries(this->getDb());
    PtrTo_RowFolderRecord result = queries.selectRowByDirectory(parentDir + suffixDir);

    return result;
}

PtrTo_RowFileRecord RowInserter::insertActiveFile(const QString &fileName,
                                                  const QString &fileExtension,
                                                  const QString &symbolDiectory,
                                                  const QString &userDirectory,
                                                  bool isAutoSyncEnabled)
{
    QSqlQuery query(this->getDb());
    QString queryTemplate = "INSERT INTO %1 (%2, %3, %4, %5, %6, %7) VALUES (:2, :3, :4, :5, :6, :7);" ;

    queryTemplate = queryTemplate.arg(TABLE_NAME_FILE_RECORD,                           // 1
                                      TABLE_FILE_RECORD_COLNAME_FILE_NAME,              // 2
                                      TABLE_FILE_RECORD_COLNAME_FILE_EXTENSION,         // 3
                                      TABLE_FILE_RECORD_COLNAME_SYMBOL_DIRECTORY,       // 4
                                      TABLE_FILE_RECORD_COLNAME_USER_DIRECTORY,         // 5
                                      TABLE_FILE_RECORD_COLNAME_IS_FROZEN,              // 6
                                      TABLE_FILE_RECORD_COLNAME_IS_AUTO_SYNC_ENABLED);  // 7

    query.prepare(queryTemplate);
    query.bindValue(":2", fileName);
    query.bindValue(":3", fileExtension);
    query.bindValue(":4", symbolDiectory);
    query.bindValue(":5", userDirectory);
    query.bindValue(":6", false);
    query.bindValue(":7", isAutoSyncEnabled);

    query.exec();
    this->setLastError(query.lastError());

    QueryFileRecord queries(this->getDb());
    auto result = queries.selectRowBySymbolFilePath(symbolDiectory + fileName + fileExtension);
    return result;
}

PtrTo_RowFileRecord RowInserter::insertFrozenFile(const QString &fileName,
                                                  const QString &fileExtension,
                                                  const QString &symbolDiectory,
                                                  bool isAutoSyncEnabled,
                                                  const QString &userDirectory)
{
    QSqlQuery query(this->getDb());
    QString queryTemplate = "INSERT INTO %1 (%2, %3, %4, %5, %6, %7) VALUES (:2, :3, :4, :5, :6, :7);" ;

    if(userDirectory.isEmpty())
        queryTemplate = "INSERT INTO %1 (%2, %3, %4, %5, %6) VALUES (:2, :3, :4, NULL, :6);" ;

    queryTemplate = queryTemplate.arg(TABLE_NAME_FILE_RECORD,                           // 1
                                      TABLE_FILE_RECORD_COLNAME_FILE_NAME,              // 2
                                      TABLE_FILE_RECORD_COLNAME_FILE_EXTENSION,         // 3
                                      TABLE_FILE_RECORD_COLNAME_SYMBOL_DIRECTORY,       // 4
                                      TABLE_FILE_RECORD_COLNAME_USER_DIRECTORY,         // 5
                                      TABLE_FILE_RECORD_COLNAME_IS_FROZEN,              // 6
                                      TABLE_FILE_RECORD_COLNAME_IS_AUTO_SYNC_ENABLED);  // 7

    query.prepare(queryTemplate);
    query.bindValue(":2", fileName);
    query.bindValue(":3", fileExtension);
    query.bindValue(":4", symbolDiectory);

    if(!userDirectory.isEmpty())
        query.bindValue(":5", userDirectory);

    query.bindValue(":6", true);
    query.bindValue(":7", isAutoSyncEnabled);

    query.exec();
    this->setLastError(query.lastError());

    QueryFileRecord queries(this->getDb());
    auto result = queries.selectRowBySymbolFilePath(symbolDiectory + fileName + fileExtension);
    return result;
}

PtrTo_RowFileVersion RowInserter::insertFileVersion(qlonglong parentRecordID,
                                                    qlonglong versionNumber,
                                                    const QString &internalFileName,
                                                    qlonglong size,
                                                    const QString &hash,
                                                    const QString &description,
                                                    const QDateTime &timestamp)
{
    QSqlQuery query(this->getDb());
    QString queryTemplate = "INSERT INTO %1 (%2, %3, %4, %5, %6, %7, %8) "
                            "VALUES(:2, :3, :4, :5, :6, :7, :8);" ;

    queryTemplate = queryTemplate.arg(TABLE_NAME_FILE_VERSION,                        // 1
                                      TABLE_FILE_VERSION_COLNAME_PARENT_RECORD_ID,    // 2
                                      TABLE_FILE_VERSION_COLNAME_VERSION_NUMBER,      // 3
                                      TABLE_FILE_VERSION_COLNAME_INTERNAL_FILE_NAME,  // 4
                                      TABLE_FILE_VERSION_COLNAME_SIZE,                // 5
                                      TABLE_FILE_VERSION_COLNAME_TIMESTAMP,           // 6
                                      TABLE_FILE_VERSION_COLNAME_DESCRIPTION,         // 7
                                      TABLE_FILE_VERSION_COLNAME_HASH);               // 8

    query.prepare(queryTemplate);
    query.bindValue(":2", parentRecordID);
    query.bindValue(":3", versionNumber);
    query.bindValue(":4", internalFileName);
    query.bindValue(":5", size);
    query.bindValue(":6", timestamp);
    query.bindValue(":7", description);
    query.bindValue(":8", hash);
    query.exec();

    this->setLastError(query.lastError());

    QueryFileVersion queries(this->getDb());

    auto result = queries.selectRowByCompositeKey(parentRecordID, versionNumber);

    return result;
}

PtrTo_RowFileEvent RowInserter::insertUnRegisteredFileEvent(const QString &internalFileName)
{
    QString queryTemplate = "INSERT INTO %1 (%2, %3) "
                            "VALUES(:2, :3);" ;

    queryTemplate = queryTemplate.arg(TABLE_NAME_FILE_EVENT,           // 1
                                      TABLE_FILE_EVENT_COLNAME_TYPE,   // 2
                                      TABLE_FILE_EVENT_COLNAME_DATA);  // 3

    QSqlQuery query(this->getDb());

    query.prepare(queryTemplate);
    query.bindValue(":2", FileEventType::UnRegisteredFileEvent);
    query.bindValue(":3", internalFileName);

    query.exec();

    this->setLastError(query.lastError());

    QueryFileEvent queries(this->getDb());

    auto result = queries.selectRowByEventID(query.lastInsertId().toLongLong());

    return result;
}

PtrTo_RowSaveGroupItem RowInserter::insertSaveGroupItem(qlonglong saveGroupID, qlonglong versionID, const QString &originalDescription)
{
    QString queryTemplate = "INSERT INTO %1 (%2, %3, %4) VALUES (:2, :3, NULL);" ;

    if(!originalDescription.isEmpty())
        queryTemplate = "INSERT INTO %1 (%2, %3, %4) VALUES (:2, :3, :4);" ;

    queryTemplate = queryTemplate.arg(TABLE_NAME_SAVE_GROUP_ITEM,                           // 1
                                      TABLE_SAVE_GROUP_ITEM_COLNAME_SAVE_GROUP_ID,          // 2
                                      TABLE_SAVE_GROUP_ITEM_COLNAME_PARENT_VERSION_ID,      // 3
                                      TABLE_SAVE_GROUP_ITEM_COLNAME_ORIGINAL_DESCRIPTION);  // 4

    QSqlQuery query(this->getDb());

    query.prepare(queryTemplate);
    query.bindValue(":2", saveGroupID);
    query.bindValue(":3", versionID);

    if(!originalDescription.isEmpty())
        query.bindValue(":4", originalDescription);

    query.exec();
    this->setLastError(query.lastError());

    QuerySaveGroupItem queries(this->getDb());
    PtrTo_RowSaveGroupItem result = queries.selectRowByID(query.lastInsertId().toLongLong());

    return result;
}
