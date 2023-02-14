#include "ItemDelegateAction.h"

#include "TreeItem.h"

#include <QQueue>
#include <QComboBox>

const QString ItemDelegateAction::ITEM_TEXT_SAVE = tr("Save");
const QString ItemDelegateAction::ITEM_TEXT_RESTORE = tr("Restore");
const QString ItemDelegateAction::ITEM_TEXT_FREEZE = tr("Freeze");
const QString ItemDelegateAction::ITEM_TEXT_DELETE = tr("Delete from Db");

ItemDelegateAction::ItemDelegateAction(QObject *parent)
    : QStyledItemDelegate(parent)
{

}

ItemDelegateAction::~ItemDelegateAction()
{

}

QWidget *ItemDelegateAction::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QComboBox *result = new QComboBox(parent);
    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    FileSystemEventDb::ItemStatus status = item->getStatus();

    if(status == FileSystemEventDb::ItemStatus::NewAdded)
    {
        item->setAction(TreeItem::Action::Save);
        result->addItem(ITEM_TEXT_SAVE);
    }
    else if(status == FileSystemEventDb::ItemStatus::Updated ||
            status == FileSystemEventDb::ItemStatus::Renamed ||
            status == FileSystemEventDb::ItemStatus::UpdatedAndRenamed)
    {
        item->setAction(TreeItem::Action::Save);
        result->addItem(ITEM_TEXT_SAVE);
        result->addItem(ITEM_TEXT_RESTORE);
    }
    else if(status == FileSystemEventDb::ItemStatus::Deleted)
    {
        item->setAction(TreeItem::Action::Delete);
        result->addItem(ITEM_TEXT_DELETE);
        result->addItem(ITEM_TEXT_RESTORE);
        result->addItem(ITEM_TEXT_FREEZE);
    }
    else if(status == FileSystemEventDb::ItemStatus::Missing)
    {
        item->setAction(TreeItem::Action::Restore);
        result->addItem(ITEM_TEXT_RESTORE);
        result->addItem(ITEM_TEXT_DELETE);
        result->addItem(ITEM_TEXT_FREEZE);
    }
    else
    {
        delete result;
        result = nullptr;
    }

    // Don't add ItemDelegateAction for children of deleted or missing folder
    if(item->getParentItem() != nullptr &&
       item->getParentItem()->getType() == TreeItem::ItemType::Folder)
    {
        if(item->getParentItem()->getStatus() == FileSystemEventDb::ItemStatus::Deleted ||
           item->getParentItem()->getStatus() == FileSystemEventDb::ItemStatus::Missing)
        {
            delete result;
            result = nullptr;
        }
    }

    if(result != nullptr)
    {
        // This lambda connection causes immediate trigger of ItemDelegateDescription::setModelData()
        QObject::connect(result, &QComboBox::textActivated,
                         this, [=](const QString &item){
            Q_UNUSED(item);
            result->clearFocus();
        });
    }

    return result;
}

void ItemDelegateAction::setEditorData(QWidget *editor, const QModelIndex &index) const
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

void ItemDelegateAction::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *cb = qobject_cast<QComboBox *>(editor);
    Q_ASSERT(cb);

    TreeItem *item = static_cast<TreeItem *>(index.internalPointer());

    TreeItem::Action action;

    if(cb->currentText() == ITEM_TEXT_SAVE)
        action = TreeItem::Action::Save;

    else if(cb->currentText() == ITEM_TEXT_RESTORE)
        action = TreeItem::Action::Restore;

    else if(cb->currentText() == ITEM_TEXT_FREEZE)
        action = TreeItem::Action::Freeze;

    else if(cb->currentText() == ITEM_TEXT_DELETE)
        action = TreeItem::Action::Delete;

    else
        action = TreeItem::Action::NotSelected;

    item->setAction(action);

    // Update action for children of deleted or missing folder
    if(item->getType() == TreeItem::ItemType::Folder)
    {
        if(item->getStatus() == FileSystemEventDb::ItemStatus::Deleted ||
           item->getStatus() == FileSystemEventDb::ItemStatus::Missing)
        {
            QQueue<TreeItem *> children;

            for(int index = 0; index < item->childCount(); index++)
                children.enqueue(item->child(index));

            while(!children.isEmpty())
            {
                TreeItem *current = children.dequeue();
                current->setAction(action);

                for(int i = 0; i < current->childCount(); i++)
                    children.enqueue(current->child(i));
            }
        }
    }
}
