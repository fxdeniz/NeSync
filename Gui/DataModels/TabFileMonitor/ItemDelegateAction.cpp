#include "ItemDelegateAction.h"

#include "TreeModelFileMonitor.h"
#include "TreeItem.h"

#include <QQueue>
#include <QComboBox>

const QString ItemDelegateAction::ITEM_TEXT_SAVE = tr("Save");
const QString ItemDelegateAction::ITEM_TEXT_RESTORE = tr("Restore");
const QString ItemDelegateAction::ITEM_TEXT_FREEZE = tr("Freeze");
const QString ItemDelegateAction::ITEM_TEXT_DELETE = tr("Delete from Db");

const QString ItemDelegateAction::ITEM_TEXT_SAVE_WITH_CHILDREN = tr("Save (with children)");
const QString ItemDelegateAction::ITEM_TEXT_RESTORE_WITH_CHILDREN = tr("Restore (with children)");
const QString ItemDelegateAction::ITEM_TEXT_FREEZE_WITH_CHILDREN = tr("Freeze (with children)");
const QString ItemDelegateAction::ITEM_TEXT_DELETE_WITH_CHILDREN = tr("Delete from Db (with children)");

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

        if(item->getType() == TreeItem::ItemType::File)
            result->addItem(ITEM_TEXT_SAVE);
        else if(item->getType() == TreeItem::ItemType::Folder)
            result->addItem(ITEM_TEXT_SAVE_WITH_CHILDREN);
    }
    else if(status == FileSystemEventDb::ItemStatus::Updated ||
            status == FileSystemEventDb::ItemStatus::Renamed ||
            status == FileSystemEventDb::ItemStatus::UpdatedAndRenamed)
    {
        item->setAction(TreeItem::Action::Save);

        if(item->getType() == TreeItem::ItemType::File)
        {
            result->addItem(ITEM_TEXT_SAVE);
            result->addItem(ITEM_TEXT_RESTORE);
        }
        else if(item->getType() == TreeItem::ItemType::Folder)
        {
            result->addItem(ITEM_TEXT_SAVE_WITH_CHILDREN);
            result->addItem(ITEM_TEXT_RESTORE_WITH_CHILDREN);
        }
    }
    else if(status == FileSystemEventDb::ItemStatus::Deleted)
    {
        item->setAction(TreeItem::Action::Delete);

        if(item->getType() == TreeItem::ItemType::File)
        {
            result->addItem(ITEM_TEXT_DELETE);
            result->addItem(ITEM_TEXT_RESTORE);
            result->addItem(ITEM_TEXT_FREEZE);
        }
        else if(item->getType() == TreeItem::ItemType::Folder)
        {
            result->addItem(ITEM_TEXT_DELETE_WITH_CHILDREN);
            result->addItem(ITEM_TEXT_RESTORE_WITH_CHILDREN);
            result->addItem(ITEM_TEXT_FREEZE_WITH_CHILDREN);
        }
    }
    else if(status == FileSystemEventDb::ItemStatus::Missing)
    {
        item->setAction(TreeItem::Action::Restore);

        if(item->getType() == TreeItem::ItemType::File)
        {
            result->addItem(ITEM_TEXT_RESTORE);
            result->addItem(ITEM_TEXT_DELETE);
            result->addItem(ITEM_TEXT_FREEZE);
        }
        else if(item->getType() == TreeItem::ItemType::Folder)
        {
            result->addItem(ITEM_TEXT_RESTORE_WITH_CHILDREN);
            result->addItem(ITEM_TEXT_DELETE_WITH_CHILDREN);
            result->addItem(ITEM_TEXT_FREEZE_WITH_CHILDREN);
        }
    }
    else
    {
        delete result;
        result = nullptr;
    }

    // Don't add ItemDelegateAction for children of deleted, missing or new added folder
    if(item->getParentItem() != nullptr &&
       item->getParentItem()->getType() == TreeItem::ItemType::Folder)
    {
        if(item->getParentItem()->getStatus() == FileSystemEventDb::ItemStatus::Deleted ||
           item->getParentItem()->getStatus() == FileSystemEventDb::ItemStatus::Missing ||
           item->getParentItem()->getStatus() == FileSystemEventDb::ItemStatus::NewAdded)
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

        auto treeModel = (TreeModelFileMonitor *) index.model();
        QObject::connect(treeModel, &TreeModelFileMonitor::signalDisableItemDelegates,
                         result, [=]{
            result->setDisabled(true);
        });
    }

    return result;
}

void ItemDelegateAction::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QComboBox *comboBox = qobject_cast<QComboBox *>(editor);
    Q_ASSERT(comboBox);
    // get the index of the text in the combobox that matches the current value of the item
    const QString currentText = index.data(Qt::EditRole).toString();
    const int cbIndex = comboBox->findText(currentText);
    // if it is valid, adjust the combobox
    if (cbIndex >= 0)
        comboBox->setCurrentIndex(cbIndex);
}

void ItemDelegateAction::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *comboBox = qobject_cast<QComboBox *>(editor);
    Q_ASSERT(comboBox);

    TreeItem *item = static_cast<TreeItem *>(index.internalPointer());
    TreeItem::Action action;
    QString currentText = comboBox->currentText();

    if(currentText == ITEM_TEXT_SAVE || currentText == ITEM_TEXT_SAVE_WITH_CHILDREN)
        action = TreeItem::Action::Save;

    else if(currentText == ITEM_TEXT_RESTORE || currentText == ITEM_TEXT_RESTORE_WITH_CHILDREN)
        action = TreeItem::Action::Restore;

    else if(currentText == ITEM_TEXT_FREEZE || currentText == ITEM_TEXT_FREEZE_WITH_CHILDREN)
        action = TreeItem::Action::Freeze;

    else if(currentText == ITEM_TEXT_DELETE || currentText == ITEM_TEXT_DELETE_WITH_CHILDREN)
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
