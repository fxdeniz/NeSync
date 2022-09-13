#include "ComboBoxItemDelegateFileAction.h"
#include "DataModels/TabFileMonitor/TableModelFileMonitor.h"

#include <QComboBox>

const QString ComboBoxItemDelegateFileAction::ITEM_TEXT_SAVE = tr("Save");
const QString ComboBoxItemDelegateFileAction::ITEM_TEXT_RESTORE = tr("Restore");
const QString ComboBoxItemDelegateFileAction::ITEM_TEXT_FREEZE = tr("Freeze");
const QString ComboBoxItemDelegateFileAction::ITEM_TEXT_DELETE = tr("Delete from Db");

ComboBoxItemDelegateFileAction::ComboBoxItemDelegateFileAction(QObject *parent)
    : QStyledItemDelegate(parent)
{
}


ComboBoxItemDelegateFileAction::~ComboBoxItemDelegateFileAction()
{
}


QWidget *ComboBoxItemDelegateFileAction::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // Create the combobox and populate it
    QComboBox *cb = new QComboBox(parent);

    QModelIndex statusIndex = index.siblingAtColumn(TableModelFileMonitor::ColumnIndex::Status);
    auto statusText = statusIndex.data().toString();
    auto statusCode = TableModelFileMonitor::statusCodeFromString(statusText);

    if(statusCode == TableModelFileMonitor::ItemStatus::Missing)
    {
        cb->addItem(ITEM_TEXT_RESTORE);
        cb->addItem(ITEM_TEXT_FREEZE);
        cb->addItem(ITEM_TEXT_DELETE);
    }
    else if(statusCode == TableModelFileMonitor::ItemStatus::NewAdded)
    {
        cb->addItem(ITEM_TEXT_SAVE);
    }
    else if(statusCode == TableModelFileMonitor::ItemStatus::Modified ||
            statusCode == TableModelFileMonitor::ItemStatus::Moved ||
            statusCode == TableModelFileMonitor::ItemStatus::MovedAndModified)
    {
        cb->addItem(ITEM_TEXT_SAVE);
        cb->addItem(ITEM_TEXT_RESTORE);

        QModelIndex progressIndex = index.siblingAtColumn(TableModelFileMonitor::ColumnIndex::Progress);
        auto progressCodeText = progressIndex.data().toString();
        auto progressCode = TableModelFileMonitor::progressStatusCodeFromString(progressCodeText);

        if(progressCode == TableModelFileMonitor::ProgressStatus::ApplyingAutoAction)
        {
            cb->setCurrentIndex(0);
            cb->setDisabled(true);
        }
    }
    else if(statusCode == TableModelFileMonitor::ItemStatus::Deleted)
    {
        cb->addItem(ITEM_TEXT_DELETE);
        cb->addItem(ITEM_TEXT_RESTORE);
        cb->addItem(ITEM_TEXT_FREEZE);
    }

    return cb;
}


void ComboBoxItemDelegateFileAction::setEditorData(QWidget *editor, const QModelIndex &index) const
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


void ComboBoxItemDelegateFileAction::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *cb = qobject_cast<QComboBox *>(editor);
    Q_ASSERT(cb);

    auto tableModel = qobject_cast<TableModelFileMonitor *>(model);
    QModelIndex indexPath = index.siblingAtColumn(TableModelFileMonitor::ColumnIndex::Path);

    enum TableModelFileMonitor::Action action;

    if(cb->currentText() == ITEM_TEXT_SAVE)
        action = TableModelFileMonitor::Action::Save;
    else if(cb->currentText() == ITEM_TEXT_RESTORE)
        action = TableModelFileMonitor::Action::Restore;
    else if(cb->currentText() == ITEM_TEXT_FREEZE)
        action = TableModelFileMonitor::Action::Freeze;
    else if(cb->currentText() == ITEM_TEXT_DELETE)
        action = TableModelFileMonitor::Action::Delete;
    else
        action = TableModelFileMonitor::Action::UndefinedAction;

    tableModel->saveActionContentOfRow(indexPath.data().toString(), action);

    // TODO make new designs use line below.
    // model->setData(index, cb->currentText(), Qt::EditRole);
}
