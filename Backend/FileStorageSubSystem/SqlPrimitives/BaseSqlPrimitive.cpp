#include "BaseSqlPrimitive.h"

#include <QSqlQuery>
#include <QSqlRecord>

BaseSqlPrimitive::BaseSqlPrimitive(const QSqlDatabase db, const QString &tableName)
{
    this->setDb(db);
    this->setTableName(tableName);
}

const QSqlDatabase &BaseSqlPrimitive::getDb() const
{
    return db;
}

void BaseSqlPrimitive::setDb(const QSqlDatabase &newDb)
{
    this->db = newDb;
}

const QString &BaseSqlPrimitive::getTableName() const
{
    return tableName;
}

void BaseSqlPrimitive::setTableName(const QString &newTableName)
{
    tableName = newTableName;
}

const QSqlError &BaseSqlPrimitive::getLastError() const
{
    return lastError;
}

BaseSqlPrimitive::~BaseSqlPrimitive()
{

}

void BaseSqlPrimitive::setLastError(const QSqlError &newLastError)
{
    lastError = newLastError;
}

