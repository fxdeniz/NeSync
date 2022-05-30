#ifndef ROWDELETER_H
#define ROWDELETER_H

#include "BaseSqlPrimitive.h"
#include "BaseRow.h"

class RowDeleter : public BaseSqlPrimitive
{
public:
    RowDeleter(QSharedPointer<BaseRow> targetRow);

    bool deleteRow();

private:
    QSharedPointer<BaseRow> &getTargetRow();
    bool executeDeleteQueryByType();
    bool isRowTypeOf(QSharedPointer<const BaseRow> row, const QString &typeName) const;
    bool queryTemplateDeleteRow(const QString &tableName, const QString &colName);
    void markTargetRowAsDeleted();

    QSharedPointer<BaseRow> targetRow;
};

#endif // ROWDELETER_H
