#include "InsertSaveGroupItem.h"
#include "QuerySaveGroupItem.h"

#include <QSqlQuery>
#include <QSqlRecord>

InsertSaveGroupItem::InsertSaveGroupItem(const QSqlDatabase &db) : BaseSqlPrimitive(db, TABLE_NAME_SAVE_GROUP_ITEM)
{
}

PtrTo_RowSaveGroupItem InsertSaveGroupItem::insertInto(qlonglong saveGroupID,
                                                 qlonglong versionID,
                                                 const QString &originalDescription)
{
    QString queryTemplate = "INSERT INTO %1 (%2, %3, %4) VALUES (:2, :3, NULL);" ;

    if(!originalDescription.isEmpty())
        queryTemplate = "INSERT INTO %1 (%2, %3, %4) VALUES (:2, :3, :4);" ;

    queryTemplate = queryTemplate.arg(this->getTableName(),                                 // 1
                                      TABLE_SAVE_GROUP_ITEM_COLNAME_SAVE_GROUP_ID,          // 2
                                      TABLE_SAVE_GROUP_ITEM_COLNAME_PARENT_VERSION_ID,             // 3
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
    PtrTo_RowSaveGroupItem result = queries.getRowByID(query.lastInsertId().toLongLong());

    return result;
}
