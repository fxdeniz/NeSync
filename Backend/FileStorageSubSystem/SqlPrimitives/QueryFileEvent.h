#ifndef QUERYFILEEVENT_H
#define QUERYFILEEVENT_H

#include "BaseSqlPrimitive.h"
#include "TableInfoFileEvent.h"
#include "RowFileEvent.h"

class QueryFileEvent : public BaseSqlPrimitive
{
public:
    QueryFileEvent(const QSqlDatabase &db);

    PtrTo_RowFileEvent selectRowByEventID(qlonglong eventID) const;
    QList<RowFileEvent> selectAllUnRegisteredFileEvents() const;

private:
    template <typename T>
    PtrTo_RowFileEvent queryTemplateSelectRowByKey(const QString &keyColumnName, T keyValue) const;
};

#endif // QUERYFILEEVENT_H
