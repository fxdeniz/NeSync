#ifndef ROWSAVEGROUPITEM_H
#define ROWSAVEGROUPITEM_H

#include "BaseRow.h"
#include "TableInfoSaveGroupItem.h"
#include "TableInfoFileVersion.h"

#include <QDateTime>

class RowSaveGroupItem : public BaseRow
{
public:
    friend class QuerySaveGroupItem;

    RowSaveGroupItem();

    qlonglong getSaveGroupID() const;
    qlonglong getParentVersionID() const;
    const QDateTime &getOriginalTimestamp() const;
    const QString &getOriginalDescription() const;
    PtrTo_RowFileVersion getParentRowFileVersion() const;

   QString toString() const override;

private:
    RowSaveGroupItem(const QSqlDatabase &db, const QSqlRecord &record);

    qlonglong saveGroupID;
    qlonglong parentVersionID;
    QDateTime originalTimestamp;
    QString originalDescription;
};

#endif // ROWSAVEGROUPITEM_H
