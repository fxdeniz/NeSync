#ifndef INSERTSAVEGROUPITEM_H
#define INSERTSAVEGROUPITEM_H

#include "BaseSqlPrimitive.h"
#include "RowSaveGroupItem.h"

class InsertSaveGroupItem : public BaseSqlPrimitive
{
public:
    InsertSaveGroupItem(const QSqlDatabase &db);

    PtrTo_RowSaveGroupItem insertInto(qlonglong saveGroupID,
                                   qlonglong versionID,
                                   const QString &originalDescription = "");
};

#endif // INSERTSAVEGROUPITEM_H
