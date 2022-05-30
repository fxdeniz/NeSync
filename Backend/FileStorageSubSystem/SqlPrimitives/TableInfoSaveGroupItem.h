#ifndef TABLEINFOSAVEGROUPITEM_H
#define TABLEINFOSAVEGROUPITEM_H

#include <QString>
#include <QSharedPointer>

inline const QString TABLE_NAME_SAVE_GROUP_ITEM = "SaveGroupItem";
inline const QString TABLE_SAVE_GROUP_ITEM_COLNAME_ITEM_ID = "item_id";
inline const QString TABLE_SAVE_GROUP_ITEM_COLNAME_SAVE_GROUP_ID = "save_group_id";
inline const QString TABLE_SAVE_GROUP_ITEM_COLNAME_PARENT_VERSION_ID = "parent_version_id";
inline const QString TABLE_SAVE_GROUP_ITEM_COLNAME_ORIGINAL_TIMESTAMP = "original_timestamp";
inline const QString TABLE_SAVE_GROUP_ITEM_COLNAME_ORIGINAL_DESCRIPTION = "original_description";

class RowSaveGroupItem;

typedef QSharedPointer<RowSaveGroupItem> PtrTo_RowSaveGroupItem;
typedef QSharedPointer<const RowSaveGroupItem> PtrToConst_RowSaveGroupItem;
typedef const QSharedPointer<RowSaveGroupItem> ConstPtrTo_RowSaveGroupItem;
typedef const QSharedPointer<const RowSaveGroupItem> ConstPtrToConst_RowSaveGroupItem;

#endif // TABLEINFOSAVEGROUPITEM_H
