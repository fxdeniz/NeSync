#include "RowSaveGroupItem.h"

#include "QueryFileVersion.h"

#include <QSqlRecord>

RowSaveGroupItem::RowSaveGroupItem() : BaseRow(TABLE_NAME_SAVE_GROUP_ITEM)
{
    this->saveGroupID = INVALID_FIELD_VALUE_QLONGLONG;
    this->parentVersionID = INVALID_FIELD_VALUE_QLONGLONG;
    this->originalTimestamp = INVALID_FIELD_VALUE_QDATETIME;
    this->originalDescription = INVALID_FIELD_VALUE_QSTRING;
}

RowSaveGroupItem::RowSaveGroupItem(const QSqlDatabase &db, const QSqlRecord &record)
    : BaseRow(db, record.value(TABLE_SAVE_GROUP_ITEM_COLNAME_ITEM_ID).toLongLong(), TABLE_NAME_SAVE_GROUP_ITEM)
{
    this->saveGroupID = record.value(TABLE_SAVE_GROUP_ITEM_COLNAME_SAVE_GROUP_ID).toLongLong();
    this->parentVersionID = record.value(TABLE_SAVE_GROUP_ITEM_COLNAME_PARENT_VERSION_ID).toLongLong();
    this->originalTimestamp = record.value(TABLE_SAVE_GROUP_ITEM_COLNAME_ORIGINAL_TIMESTAMP).toDateTime();
    this->originalDescription = record.value(TABLE_SAVE_GROUP_ITEM_COLNAME_ORIGINAL_DESCRIPTION).toString();
}

qlonglong RowSaveGroupItem::getSaveGroupID() const
{
    return saveGroupID;
}

qlonglong RowSaveGroupItem::getParentVersionID() const
{
    return parentVersionID;
}

const QDateTime &RowSaveGroupItem::getOriginalTimestamp() const
{
    return originalTimestamp;
}

const QString &RowSaveGroupItem::getOriginalDescription() const
{
    return originalDescription;
}

PtrTo_RowFileVersion RowSaveGroupItem::getParentRowFileVersion() const
{
    auto result = QueryFileVersion(this->getDb()).selectRowByID(this->getParentVersionID());

    return result;
}

QString RowSaveGroupItem::toString() const
{
    QString ptrStr = QString("0x%1").arg((quintptr)this, QT_POINTER_SIZE * 2, 16, QChar('0'));
    QString result = "RowSaveGroupItem (%1) has:";
    result = result.arg(ptrStr);

    result += " \t\n Parent File Version = " + this->getParentRowFileVersion()->toString();
    result += " " + TABLE_SAVE_GROUP_ITEM_COLNAME_ITEM_ID + " = " + QString::number(this->getID());
    result += " | " + TABLE_SAVE_GROUP_ITEM_COLNAME_SAVE_GROUP_ID + " = " + QString::number(this->getSaveGroupID());
    result += " | " + TABLE_SAVE_GROUP_ITEM_COLNAME_PARENT_VERSION_ID + " = " + QString::number(this->getParentVersionID());
    result += " | " + TABLE_SAVE_GROUP_ITEM_COLNAME_ORIGINAL_TIMESTAMP + " = " + this->getOriginalTimestamp().toString();
    result += " | " + TABLE_SAVE_GROUP_ITEM_COLNAME_ORIGINAL_DESCRIPTION + " = " + this->getOriginalDescription();

    return result;
}
