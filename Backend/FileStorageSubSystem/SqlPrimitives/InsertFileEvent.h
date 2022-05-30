#ifndef INSERTFILEEVENT_H
#define INSERTFILEEVENT_H

#include "BaseSqlPrimitive.h"
#include "RowFileEvent.h"

class InsertFileEvent : public BaseSqlPrimitive
{
public:
    InsertFileEvent(const QSqlDatabase &db);

    PtrTo_RowFileEvent insertUnRegisteredFileEvent(const QString &internalFileName);

};

#endif // INSERTFILEEVENT_H
