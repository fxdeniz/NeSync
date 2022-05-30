#include "QuerySaveGroupItem.h"

#include <QSqlQuery>
#include <QSqlRecord>

QuerySaveGroupItem::QuerySaveGroupItem(const QSqlDatabase &db) : BaseSqlPrimitive(db, TABLE_NAME_SAVE_GROUP_ITEM)
{
}

PtrTo_RowSaveGroupItem QuerySaveGroupItem::selectRowByID(qlonglong itemID) const
{
    auto result = this->queryTemplateSelectRowByKey<qlonglong>(TABLE_SAVE_GROUP_ITEM_COLNAME_ITEM_ID, itemID);

    return result;
}

PtrTo_RowSaveGroupItem QuerySaveGroupItem::selectRowByParentFileVerisonID(qlonglong versionID) const
{
    auto result = this->queryTemplateSelectRowByKey<qlonglong>(TABLE_SAVE_GROUP_ITEM_COLNAME_PARENT_VERSION_ID, versionID);

    return result;
}

QList<PtrTo_RowSaveGroupItem> QuerySaveGroupItem::selectRowsInSaveGroup(qlonglong saveGroupID) const
{
    QList<PtrTo_RowSaveGroupItem> result;

    QString queryTemplate = "SELECT * FROM %1 WHERE %2 = :2;" ;
    queryTemplate = queryTemplate.arg(this->getTableName(), TABLE_SAVE_GROUP_ITEM_COLNAME_SAVE_GROUP_ID);

    QSqlQuery query(this->getDb());

    query.prepare(queryTemplate);
    query.bindValue(":2", saveGroupID);

    query.exec();

    while(query.next())
    {
        RowSaveGroupItem *ptr = new RowSaveGroupItem(this->getDb(), query.record());
        PtrTo_RowSaveGroupItem item(ptr);
        result.append(item);
    }

    return result;
}

qlonglong QuerySaveGroupItem::selectCurrentSaveGroupID() const
{
    QString result_column = "result_column";

    QString queryTemplate = "SELECT MAX(%1) AS %2 FROM %3;" ;
    queryTemplate = queryTemplate.arg(TABLE_SAVE_GROUP_ITEM_COLNAME_SAVE_GROUP_ID,  // 1
                                      result_column,                                // 2
                                      this->getTableName());                        // 3

    QSqlQuery query(this->getDb());

    query.prepare(queryTemplate);
    query.exec();

    if(query.isNull(result_column))
        return 1;

    qlonglong columnValue = query.record().value(result_column).toLongLong();

    return columnValue;
}

QList<qlonglong> QuerySaveGroupItem::selectSaveGroupIDList() const
{
    QList<qlonglong> result;

    QString queryTemplate = "SELECT DISTINCT %1 FROM %2 ORDER BY %1 ASC;" ;
    queryTemplate = queryTemplate.arg(TABLE_SAVE_GROUP_ITEM_COLNAME_SAVE_GROUP_ID, this->getTableName());

    QSqlQuery query(this->getDb());
    query.exec(queryTemplate);

    while(query.next())
    {
        auto value = query.record().value(TABLE_SAVE_GROUP_ITEM_COLNAME_SAVE_GROUP_ID).toLongLong();
        result.append(value);
    }

    return result;
}

template<typename T>
PtrTo_RowSaveGroupItem QuerySaveGroupItem::queryTemplateSelectRowByKey(const QString &keyColumnName,
                                                                       T keyValue) const
{
    QString queryTemplate = "SELECT * FROM %1 WHERE %2 = :2;" ;
    queryTemplate = queryTemplate.arg(this->getTableName(),   // 1
                                      keyColumnName);         // 2

    QSqlQuery query(this->getDb());
    query.prepare(queryTemplate);

    query.bindValue(":2", keyValue);

    query.exec();

    RowSaveGroupItem *ptr = nullptr;

    if(!query.next())
        ptr = new RowSaveGroupItem();
    else
        ptr = new RowSaveGroupItem(this->getDb(), query.record());

    PtrTo_RowSaveGroupItem result(ptr);

    return result;
}
