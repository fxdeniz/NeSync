#include "BaseRow.h"

#include <QSqlQuery>

BaseRow::BaseRow() : BaseSqlPrimitive(QSqlDatabase(), INVALID_FIELD_VALUE_QSTRING)
{
    this->setExistInDB(false);
    this->rowID = INVALID_FIELD_VALUE_QLONGLONG;
}

BaseRow::BaseRow(const QString &tableName) : BaseSqlPrimitive(QSqlDatabase(), tableName)
{
    this->setExistInDB(false);
    this->rowID = INVALID_FIELD_VALUE_QLONGLONG;
}

BaseRow::BaseRow(const QSqlDatabase &db,
                 qlonglong rowID,
                 const QString &tableName) : BaseSqlPrimitive(db, tableName)
{
    this->setExistInDB(true);
    this->rowID = rowID;
}

bool BaseRow::isExistInDB() const
{
    return existInDB;
}

QString BaseRow::toString() const
{
    return INVALID_FIELD_VALUE_QSTRING;
}

qlonglong BaseRow::getID() const
{
    return rowID;
}

void BaseRow::setExistInDB(bool newExistInDB)
{
    this->existInDB = newExistInDB;
}
