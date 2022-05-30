#include "InsertFileVersion.h"
#include "QueryFileVersion.h"

#include <QSqlQuery>
#include <QSqlRecord>

InsertFileVersion::InsertFileVersion(const QSqlDatabase &db) : BaseSqlPrimitive(db, TABLE_NAME_FILE_VERSION)
{
}

PtrTo_RowFileVersion InsertFileVersion::insertInto(qlonglong parentRecordID,
                                             qlonglong versionNumber,
                                             const QString &internalFileName,
                                             qlonglong size,
                                             const QString &hash,
                                             const QString &description,
                                             const QDateTime &timestamp)
{
    QSqlQuery query(this->getTableName());
    QString queryTemplate = "INSERT INTO %1 (%2, %3, %4, %5, %6, %7, %8) "
                            "VALUES(:2, :3, :4, :5, :6, :7, :8);" ;

    queryTemplate = queryTemplate.arg(this->getTableName(),                           // 1

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

    auto result = queries.getRowByCompositeKey(parentRecordID, versionNumber);

    return result;
}
