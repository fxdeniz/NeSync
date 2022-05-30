#include "RowDeleter.h"

#include <QSqlQuery>

#include "TableInfoSaveGroupItem.h"
#include "TableInfoFolderRecord.h"
#include "TableInfoFileVersion.h"
#include "TableInfoFileRecord.h"
#include "TableInfoFileEvent.h"


RowDeleter::RowDeleter(QSharedPointer<BaseRow> targetRow)
    : BaseSqlPrimitive(targetRow->getDb(), "")
{
    this->targetRow = targetRow;
}

QSharedPointer<BaseRow> &RowDeleter::getTargetRow()
{
    return this->targetRow;
}

bool RowDeleter::deleteRow()
{
    if(this->getTargetRow()->isExistInDB())
    {
        bool result = this->executeDeleteQueryByType();

        if(result == true)
            return true;

        return false;
    }

    return false;
}

bool RowDeleter::executeDeleteQueryByType()
{
    bool result = false;

    if(this->isRowTypeOf(this->getTargetRow(), TABLE_NAME_FOLDER_RECORD))
        result = this->queryTemplateDeleteRow(TABLE_NAME_FOLDER_RECORD, TABLE_FOLDER_RECORD_COLNAME_FOLDER_ID);

    if(this->isRowTypeOf(this->getTargetRow(), TABLE_NAME_FILE_RECORD))
        result = this->queryTemplateDeleteRow(TABLE_NAME_FILE_RECORD, TABLE_FILE_RECORD_COLNAME_RECORD_ID);

    if(this->isRowTypeOf(this->getTargetRow(), TABLE_NAME_FILE_VERSION))
        result = this->queryTemplateDeleteRow(TABLE_NAME_FILE_VERSION, TABLE_FILE_VERSION_COLNAME_VERSION_ID);

    if(this->isRowTypeOf(this->getTargetRow(), TABLE_NAME_FILE_EVENT))
        result = this->queryTemplateDeleteRow(TABLE_NAME_FILE_EVENT, TABLE_FILE_EVENT_COLNAME_EVENT_ID);

    if(this->isRowTypeOf(this->getTargetRow(), TABLE_NAME_SAVE_GROUP_ITEM))
        result = this->queryTemplateDeleteRow(TABLE_NAME_SAVE_GROUP_ITEM, TABLE_SAVE_GROUP_ITEM_COLNAME_ITEM_ID);

    return result;
}

bool RowDeleter::isRowTypeOf(QSharedPointer<const BaseRow> row, const QString &typeName) const
{
    bool result = row->getTableName() == typeName;

    return result;
}

bool RowDeleter::queryTemplateDeleteRow(const QString &tableName, const QString &colName)
{
    QString queryTemplate = "DELETE FROM %1 WHERE %2 = :2;" ;
    queryTemplate = queryTemplate.arg(tableName, colName);

    QSqlQuery query(this->getDb());

    query.prepare(queryTemplate);

    query.bindValue(":2", this->getTargetRow()->getID());

    query.exec();
    this->setLastError(query.lastError());

    if(query.numRowsAffected() >= 1
        && query.lastError().type() == QSqlError::ErrorType::NoError)
    {
        this->markTargetRowAsDeleted();
        return true;
    }

    return false;
}

void RowDeleter::markTargetRowAsDeleted()
{
    this->getTargetRow()->setExistInDB(false);
}
