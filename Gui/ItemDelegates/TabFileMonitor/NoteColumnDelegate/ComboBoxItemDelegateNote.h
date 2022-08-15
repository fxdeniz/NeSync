#ifndef COMBOBOXITEMDELEGATENOTE_H
#define COMBOBOXITEMDELEGATENOTE_H

#include <QStyledItemDelegate>

#include "IComboBoxNoteNotifier.h"

// class IComboBoxNoteNotifier;

// code here is adapted from
//      https://wiki.qt.io/Combo_Boxes_in_Item_Views
//      https://stackoverflow.com/questions/47576354/always-show-a-qcombobox-in-a-cell-of-a-qtableview
class ComboBoxItemDelegateNote : public QStyledItemDelegate
{
    Q_OBJECT
public:
    ComboBoxItemDelegateNote(IComboBoxNoteNotifier *parentTab);
    ~ComboBoxItemDelegateNote();

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

private:
    IComboBoxNoteNotifier *parentTab;
};


#endif // COMBOBOXITEMDELEGATENOTE_H
