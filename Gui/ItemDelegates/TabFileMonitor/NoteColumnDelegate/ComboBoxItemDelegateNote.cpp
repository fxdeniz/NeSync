#include "ComboBoxItemDelegateNote.h"
#include "DataModels/TabFileMonitor/TableModelFileMonitor.h"
#include "ComboBoxNote.h"

#include <QComboBox>

ComboBoxItemDelegateNote::ComboBoxItemDelegateNote(IComboBoxNoteNotifier *parentTab)
    : QStyledItemDelegate(parentTab)
{
    this->parentTab = parentTab;
}


ComboBoxItemDelegateNote::~ComboBoxItemDelegateNote()
{
}


QWidget *ComboBoxItemDelegateNote::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // Create the combobox and populate it
    ComboBoxNote *cb = new ComboBoxNote(parent);
    cb->setPlaceholderText(IComboBoxNoteNotifier::defaultNoNoteText());

    QObject::connect(parentTab, &IComboBoxNoteNotifier::signalNoteNumberAdded, cb, &ComboBoxNote::slotOnItemAdded);
    QObject::connect(parentTab, &IComboBoxNoteNotifier::signalNoteNumberDeleted, cb, &ComboBoxNote::slotOnItemRemoved);

    QModelIndex progressIndex = index.siblingAtColumn(TableModelFileMonitor::ColumnIndex::Progress);
    auto progressStatusText = progressIndex.data().toString();
    auto progressStatusCode = TableModelFileMonitor::progressStatusCodeFromString(progressStatusText);

    if(progressStatusCode == TableModelFileMonitor::ProgressStatus::ApplyingAutoAction)
        cb->setDisabled(true);

    return cb;
}


void ComboBoxItemDelegateNote::setEditorData(QWidget *editor, const QModelIndex &index) const
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


void ComboBoxItemDelegateNote::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *cb = qobject_cast<QComboBox *>(editor);
    Q_ASSERT(cb);
    model->setData(index, cb->currentText(), Qt::EditRole);
}

