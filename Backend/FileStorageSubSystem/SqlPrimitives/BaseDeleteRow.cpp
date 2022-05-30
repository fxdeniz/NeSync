#include "BaseDeleteRow.h"

#include <QSqlQuery>

BaseDeleteRow::BaseDeleteRow(const BaseRow &row, const QString &tableName) : BaseSqlPrimitive(row.getDb(), tableName)
{
    this->setTargetRow(row);
}

const BaseRow &BaseDeleteRow::getTargetRow() const
{
    return this->targetRow;
}

void BaseDeleteRow::setTargetRow(const BaseRow &newTargetRow)
{
    this->targetRow = newTargetRow;
}

bool BaseDeleteRow::deletRowByColumnName(const QString &colName)
{
    QString queryTemplate = "DELETE FROM %1 WHERE %2 = :2;" ;
    queryTemplate = queryTemplate.arg(this->getTableName(), colName);

    QSqlQuery query(this->getDb());

    query.prepare(queryTemplate);

    query.bindValue(":2", this->getTargetRow().getID());

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

void BaseDeleteRow::markTargetRowAsDeleted()
{
    this->targetRow.setExistInDB(false);
}
