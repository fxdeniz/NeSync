#include "ItemDelegateDescription.h"

#include "TreeItem.h"

#include <QComboBox>

ItemDelegateDescription::ItemDelegateDescription(QObject *parent)
    : QStyledItemDelegate(parent)
{

}

ItemDelegateDescription::~ItemDelegateDescription()
{

}

QWidget *ItemDelegateDescription::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QComboBox *result = nullptr;
    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    if(item->getType() == TreeItem::ItemType::File)
        result = new QComboBox(parent);

//    FileSystemEventDb::ItemStatus status = item->getStatus();

//    if(status == FileSystemEventDb::ItemStatus::NewAdded)
//    {
//        item->setAction(TreeItem::Action::Save);
//        result->addItem(ITEM_TEXT_SAVE);
//    }
//    else if(status == FileSystemEventDb::ItemStatus::Updated ||
//            status == FileSystemEventDb::ItemStatus::Renamed ||
//            status == FileSystemEventDb::ItemStatus::UpdatedAndRenamed)
//    {
//        item->setAction(TreeItem::Action::Save);
//        result->addItem(ITEM_TEXT_SAVE);
//        result->addItem(ITEM_TEXT_RESTORE);
//    }
//    else if(status == FileSystemEventDb::ItemStatus::Deleted)
//    {
//        item->setAction(TreeItem::Action::Delete);
//        result->addItem(ITEM_TEXT_DELETE);
//        result->addItem(ITEM_TEXT_RESTORE);
//        result->addItem(ITEM_TEXT_FREEZE);
//    }
//    else
//    {
//        delete result;
//        result = nullptr;
//    }

    return result;
}

void ItemDelegateDescription::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QComboBox *cb = qobject_cast<QComboBox *>(editor);
    Q_ASSERT(cb);
    // get the index of the text in the combobox that matches the current value of the item
    const QString currentText = index.data(Qt::EditRole).toString();
    const int cbIndex = cb->findText(currentText);
    // if it is valid, adjust the combobox
    if (cbIndex >= 0)
        cb->setCurrentIndex(cbIndex);
}

void ItemDelegateDescription::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *cb = qobject_cast<QComboBox *>(editor);
    Q_ASSERT(cb);

    TreeItem *item = static_cast<TreeItem *>(index.internalPointer());

    TreeItem::Action action;

//    if(cb->currentText() == ITEM_TEXT_SAVE)
//        action = TreeItem::Action::Save;

//    else if(cb->currentText() == ITEM_TEXT_RESTORE)
//        action = TreeItem::Action::Restore;

//    else if(cb->currentText() == ITEM_TEXT_FREEZE)
//        action = TreeItem::Action::Freeze;

//    else if(cb->currentText() == ITEM_TEXT_DELETE)
//        action = TreeItem::Action::Delete;

//    else
//        action = TreeItem::Action::NotSelected;

    item->setAction(action);
}
