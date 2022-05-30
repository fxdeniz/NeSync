#ifndef BASEROW_H
#define BASEROW_H

#include "BaseSqlPrimitive.h"

class BaseRow : public BaseSqlPrimitive
{
public:
    friend class BaseDeleteRow;
    friend class RowDeleter;

    BaseRow();
    BaseRow(const QString &tableName);
    BaseRow(const QSqlDatabase &db, qlonglong rowID, const QString &tableName);

    bool isExistInDB() const;
    virtual QString toString() const;

    qlonglong getID() const;

protected:
    void setExistInDB(bool newExistInDB);

    qlonglong rowID;
    bool existInDB;

};

#endif // BASEROW_H
