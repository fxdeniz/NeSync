#include "QueryFileVersion.h"

#include <QSqlQuery>
#include <QUuid>

QueryFileVersion::QueryFileVersion(const QSqlDatabase &db) : BaseSqlPrimitive(db, TABLE_NAME_FILE_VERSION)
{
}

PtrTo_RowFileVersion QueryFileVersion::selectRowByID(qlonglong versionID) const
{
    auto result = this->queryTemplateSelectRowByKey<qlonglong>(TABLE_FILE_VERSION_COLNAME_VERSION_ID, versionID);

    return result;
}

PtrTo_RowFileVersion QueryFileVersion::selectRowByCompositeKey(qlonglong parentRecordID,
                                                       qlonglong versionNumber) const
{
    QSqlQuery query(this->getDb());
    QString queryTemplate = "SELECT * FROM %1 WHERE %2 = :2 AND %3 = :3;" ;

    queryTemplate = queryTemplate.arg(this->getTableName(),                        // 1

                                      TABLE_FILE_VERSION_COLNAME_PARENT_RECORD_ID, // 2
                                      TABLE_FILE_VERSION_COLNAME_VERSION_NUMBER);  // 3

    query.prepare(queryTemplate);
    query.bindValue(":2", parentRecordID);
    query.bindValue(":3", versionNumber);
    query.exec();

    RowFileVersion *ptr = nullptr;

    if(!query.next())
        ptr = new RowFileVersion();
    else
        ptr = new RowFileVersion(this->getDb(), query.record());

    PtrTo_RowFileVersion result(ptr);

    return result;
}

QList<PtrTo_RowFileVersion> QueryFileVersion::selectAllVersionsOfParentRecord(qlonglong parentRecordID) const
{
    QList<PtrTo_RowFileVersion> result;

    QString queryTemplate = "SELECT * FROM %1 WHERE %2 = :2;" ;
    queryTemplate = queryTemplate.arg(this->getTableName(),                          // 1

                                      TABLE_FILE_VERSION_COLNAME_PARENT_RECORD_ID);  // 2

    QSqlQuery query(this->getDb());

    query.prepare(queryTemplate);
    query.bindValue(":2", parentRecordID);

    query.exec();

    while(query.next())
    {
        RowFileVersion *ptr = new RowFileVersion(this->getDb(), query.record());
        PtrTo_RowFileVersion item(ptr);
        result.append(item);
    }

    return result;
}

qlonglong QueryFileVersion::selectRowFileVersionCount(qlonglong parentRecordID) const
{
    QString columnName = "result_column";

    QString queryTemplate = "SELECT COUNT(*) AS %1 FROM %2 WHERE %3 = :3;" ;
    queryTemplate = queryTemplate.arg(columnName,                                    // 1
                                      this->getTableName(),                          // 2
                                      TABLE_FILE_VERSION_COLNAME_PARENT_RECORD_ID);  // 3

    QSqlQuery query(this->getDb());

    query.prepare(queryTemplate);
    query.bindValue(":3", parentRecordID);

    query.exec();

    qlonglong result = INVALID_FIELD_VALUE_QLONGLONG;

    if(query.next())
        result = query.record().value(columnName).toLongLong();

    return result;
}

QList<qlonglong> QueryFileVersion::selectVersionNumbersOfParentRecord(qlonglong parentRecordID) const
{
    QList<qlonglong> result;

    QString queryTemplate = "SELECT %1 FROM %2 WHERE %3 = :3 ORDER BY %1 ASC;" ;
    queryTemplate = queryTemplate.arg(TABLE_FILE_VERSION_COLNAME_VERSION_NUMBER,     // 1
                                      this->getTableName(),                          // 2
                                      TABLE_FILE_VERSION_COLNAME_PARENT_RECORD_ID);  // 3

    QSqlQuery query(this->getDb());

    query.prepare(queryTemplate);
    query.bindValue(":3", parentRecordID);

    query.exec();

    while(query.next())
    {
        qlonglong value = query.record().value(TABLE_FILE_VERSION_COLNAME_VERSION_NUMBER).toLongLong();
        result.append(value);
    }

    return result;
}

qlonglong QueryFileVersion::selectLatestVersionNumber(qlonglong parentRecordID) const
{
    QString columnName = "result_column";

    QString queryTemplate = "SELECT MAX(%1) AS %2 FROM %3 WHERE %4 = :4;" ;
    queryTemplate = queryTemplate.arg(TABLE_FILE_VERSION_COLNAME_VERSION_NUMBER,     // 1
                                      columnName,                                    // 2
                                      this->getTableName(),                          // 3
                                      TABLE_FILE_VERSION_COLNAME_PARENT_RECORD_ID);  // 4

    QSqlQuery query(this->getDb());

    query.prepare(queryTemplate);
    query.bindValue(":4", parentRecordID);

    query.exec();

    qlonglong result = INVALID_FIELD_VALUE_QLONGLONG;

    if(query.next())
        result = query.record().value(columnName).toLongLong();

    return result;
}

QList<PtrTo_RowFileVersion> QueryFileVersion::selectRowsByMatchingDescription(const QString &searchTerm) const
{
    QList<PtrTo_RowFileVersion> result;

    QString queryTemplate = "SELECT * FROM %1 WHERE %2 LIKE %:2%;" ;
    queryTemplate = queryTemplate.arg(this->getTableName(), TABLE_FILE_VERSION_COLNAME_DESCRIPTION);

    QSqlQuery query(this->getDb());

    query.prepare(queryTemplate);
    query.bindValue(":2", searchTerm);

    query.exec();

    while(query.next())
    {
        RowFileVersion *ptr = new RowFileVersion(this->getDb(), query.record());
        PtrTo_RowFileVersion item(ptr);
        result.append(item);
    }

    return result;
}

template<typename T>
PtrTo_RowFileVersion QueryFileVersion::queryTemplateSelectRowByKey(const QString &keyColumnName,
                                                                   T keyValue) const
{
    QString queryTemplate = "SELECT * FROM %1 WHERE %2 = :2;" ;
    queryTemplate = queryTemplate.arg(this->getTableName(),   // 1
                                      keyColumnName);         // 2

    QSqlQuery query(this->getDb());
    query.prepare(queryTemplate);

    query.bindValue(":2", keyValue);

    query.exec();

    RowFileVersion *ptr = nullptr;

    if(!query.next())
        ptr = new RowFileVersion();
    else
        ptr = new RowFileVersion(this->getDb(), query.record());

    PtrTo_RowFileVersion result = PtrTo_RowFileVersion(ptr);

    return result;
}
