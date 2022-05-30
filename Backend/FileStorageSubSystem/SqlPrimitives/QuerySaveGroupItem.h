#ifndef QUERYSAVEGROUPITEM_H
#define QUERYSAVEGROUPITEM_H

#include "RowSaveGroupItem.h"

class QuerySaveGroupItem : BaseSqlPrimitive
{
public:
    QuerySaveGroupItem(const QSqlDatabase &db);

    PtrTo_RowSaveGroupItem selectRowByID(qlonglong itemID) const;
    PtrTo_RowSaveGroupItem selectRowByParentFileVerisonID(qlonglong versionID) const;
    QList<PtrTo_RowSaveGroupItem> selectRowsInSaveGroup(qlonglong saveGroupID) const;
    qlonglong selectCurrentSaveGroupID() const;
    QList<qlonglong> selectSaveGroupIDList() const;
private:
    template <typename T>
    PtrTo_RowSaveGroupItem queryTemplateSelectRowByKey(const QString &keyColumnName, T keyValue) const;
};

#endif // QUERYSAVEGROUPITEM_H
