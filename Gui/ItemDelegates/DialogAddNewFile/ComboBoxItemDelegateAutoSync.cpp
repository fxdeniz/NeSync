#include "ComboBoxItemDelegateAutoSync.h"
#include "Dialogs/DialogAddNewFile.h"

#include <QComboBox>

ComboBoxItemDelegateAutoSync::ComboBoxItemDelegateAutoSync(DialogAddNewFile *dialog, QObject *parent)
    : QStyledItemDelegate(parent)
{
    this->dialog = dialog;
}


ComboBoxItemDelegateAutoSync::~ComboBoxItemDelegateAutoSync()
{
}


QWidget *ComboBoxItemDelegateAutoSync::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // Create the combobox and populate it
    QComboBox *cb = new QComboBox(parent);

    cb->addItem(QString("Enabled"));
    cb->addItem(QString("Disabled"));

    QObject::connect(dialog, &DialogAddNewFile::signalDisableDelegatesOfAutoSyncColumn,
                     cb, &QComboBox::setDisabled);

    return cb;
}


void ComboBoxItemDelegateAutoSync::setEditorData(QWidget *editor, const QModelIndex &index) const
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


void ComboBoxItemDelegateAutoSync::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *cb = qobject_cast<QComboBox *>(editor);
    Q_ASSERT(cb);

    bool isEnabled = false;

    if(cb->currentText() == "Enabled")
        isEnabled = true;

    model->setData(index, isEnabled, Qt::EditRole);
}

