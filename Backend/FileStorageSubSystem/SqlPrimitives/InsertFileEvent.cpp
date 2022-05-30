#include "InsertFileEvent.h"
#include "QueryFileEvent.h"

#include <QSqlQuery>
#include <QSqlRecord>

InsertFileEvent::InsertFileEvent(const QSqlDatabase &db) : BaseSqlPrimitive(db, TABLE_NAME_FILE_EVENT)
{
}

PtrTo_RowFileEvent InsertFileEvent::insertUnRegisteredFileEvent(const QString &internalFileName)
{
    QString queryTemplate = "INSERT INTO %1 (%2, %3) "
                            "VALUES(:2, :3);" ;

    queryTemplate = queryTemplate.arg(this->getTableName(),            // 1

                                      TABLE_FILE_EVENT_COLNAME_TYPE,   // 2
                                      TABLE_FILE_EVENT_COLNAME_DATA);  // 3

    QSqlQuery query(this->getDb());

    query.prepare(queryTemplate);
    query.bindValue(":2", FileEventType::UnRegisteredFileEvent);
    query.bindValue(":3", internalFileName);

    query.exec();

    this->setLastError(query.lastError());

    QueryFileEvent queries(this->getDb());

    auto result = queries.getRowByEventID(query.lastInsertId().toLongLong());

    return result;
}
