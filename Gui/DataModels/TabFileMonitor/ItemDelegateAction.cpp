#include "ItemDelegateAction.h"

#include "TreeItem.h"

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
        result->addItem(ITEM_TEXT_SAVE);

    else if(status == FileSystemEventDb::ItemStatus::Updated ||
            status == FileSystemEventDb::ItemStatus::Renamed ||
            status == FileSystemEventDb::ItemStatus::UpdatedAndRenamed)
    {
        result->addItem(ITEM_TEXT_SAVE);
        result->addItem(ITEM_TEXT_RESTORE);
    }
    else if(status == FileSystemEventDb::ItemStatus::Deleted)
    {
        result->addItem(ITEM_TEXT_DELETE);
        result->addItem(ITEM_TEXT_RESTORE);
        result->addItem(ITEM_TEXT_FREEZE);
    }
    else
    {
        delete result;
        result = nullptr;
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

//    auto tableModel = qobject_cast<TableModelFileMonitor *>(model);
//    QModelIndex indexPath = index.siblingAtColumn(TableModelFileMonitor::ColumnIndex::Path);

//    enum TableModelFileMonitor::Action action;

//    if(cb->currentText() == ITEM_TEXT_SAVE)
//        action = TableModelFileMonitor::Action::Save;
//    else if(cb->currentText() == ITEM_TEXT_RESTORE)
//        action = TableModelFileMonitor::Action::Restore;
//    else if(cb->currentText() == ITEM_TEXT_FREEZE)
//        action = TableModelFileMonitor::Action::Freeze;
//    else if(cb->currentText() == ITEM_TEXT_DELETE)
//        action = TableModelFileMonitor::Action::Delete;
//    else
//        action = TableModelFileMonitor::Action::UndefinedAction;

//    tableModel->saveActionContentOfRow(indexPath.data().toString(), action);

    // TODO make new designs use line below.
    // model->setData(index, cb->currentText(), Qt::EditRole);
}
