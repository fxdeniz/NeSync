#include "ComboBoxItemDelegateFileAction.h"
#include "DataModels/TabFileMonitor/TableModelFileMonitor.h"

#include <QComboBox>

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
        cb->addItem("Restore");
        cb->addItem("Freeze");
        cb->addItem("Delete from Db");
    }
    else if(statusCode == TableModelFileMonitor::ItemStatus::NewAdded)
    {
        cb->addItem("Save");
    }
    else if(statusCode == TableModelFileMonitor::ItemStatus::Modified ||
            statusCode == TableModelFileMonitor::ItemStatus::Moved ||
            statusCode == TableModelFileMonitor::ItemStatus::MovedAndModified)
    {
        cb->addItem("Save");
        cb->addItem("Restore");
    }
    else if(statusCode == TableModelFileMonitor::ItemStatus::Deleted)
    {
        cb->addItem("Delete from Db");
        cb->addItem("Restore");
        cb->addItem("Freeze");
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
    model->setData(index, cb->currentText(), Qt::EditRole);
}
