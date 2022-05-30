#include "QueryFileEvent.h"

#include <QSqlQuery>

QueryFileEvent::QueryFileEvent(const QSqlDatabase &db) : BaseSqlPrimitive(db, TABLE_NAME_FILE_EVENT)
{
}

PtrTo_RowFileEvent QueryFileEvent::selectRowByEventID(qlonglong eventID) const
{
    auto result = this->queryTemplateSelectRowByKey<qlonglong>(TABLE_FILE_EVENT_COLNAME_EVENT_ID, eventID);

    return result;
}

QList<RowFileEvent> QueryFileEvent::selectAllUnRegisteredFileEvents() const
{
    QString queryTemplate = "SELECT * FROM %1 WHERE %2 = :2;" ;
    queryTemplate = queryTemplate.arg(this->getTableName(), TABLE_FILE_EVENT_COLNAME_TYPE);

    QSqlQuery query(this->getDb());

    query.prepare(queryTemplate);
    query.bindValue(":2", FileEventType::UnRegisteredFileEvent);

    query.exec();

    QList<RowFileEvent> result;

    while(query.next())
        RowFileEvent event(this->getDb(), query.record());

    return result;
}

template<typename T>
PtrTo_RowFileEvent QueryFileEvent::queryTemplateSelectRowByKey(const QString &keyColumnName, T keyValue) const
{
    QString queryTemplate = "SELECT * FROM %1 WHERE %2 = :2;" ;
    queryTemplate = queryTemplate.arg(this->getTableName(),  // 1

                                      keyColumnName);        // 2

    QSqlQuery query(this->getDb());

    query.prepare(queryTemplate);
    query.bindValue(":2", keyValue);

    query.exec();

    PtrTo_RowFileEvent result(new RowFileEvent());

    if(query.next())
    {
        result.reset();
        RowFileEvent *ptr = new RowFileEvent(this->getDb(), query.record());
        result = PtrTo_RowFileEvent(ptr);
    }

    return result;
}
