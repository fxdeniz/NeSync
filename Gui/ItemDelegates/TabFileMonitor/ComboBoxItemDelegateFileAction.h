#ifndef COMBOBOXITEMDELEGATEFILEACTION_H
#define COMBOBOXITEMDELEGATEFILEACTION_H

#include <QStyledItemDelegate>

// code here is adapted from
//      https://wiki.qt.io/Combo_Boxes_in_Item_Views
//      https://stackoverflow.com/questions/47576354/always-show-a-qcombobox-in-a-cell-of-a-qtableview
class ComboBoxItemDelegateFileAction : public QStyledItemDelegate
{
    Q_OBJECT
public:
    ComboBoxItemDelegateFileAction(QObject *parent = nullptr);
    ~ComboBoxItemDelegateFileAction();


    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
};

#endif // COMBOBOXITEMDELEGATEFILEACTION_H
