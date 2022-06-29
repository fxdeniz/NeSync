#ifndef COMBOBOXITEMDELEGATENOTE_H
#define COMBOBOXITEMDELEGATENOTE_H

#include <QStyledItemDelegate>

// code here is adapted from
//      https://wiki.qt.io/Combo_Boxes_in_Item_Views
//      https://stackoverflow.com/questions/47576354/always-show-a-qcombobox-in-a-cell-of-a-qtableview
class ComboBoxItemDelegateNote : public QStyledItemDelegate
{
    Q_OBJECT
public:
    ComboBoxItemDelegateNote(QObject *parent = nullptr);
    ~ComboBoxItemDelegateNote();


    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
};


#endif // COMBOBOXITEMDELEGATENOTE_H
