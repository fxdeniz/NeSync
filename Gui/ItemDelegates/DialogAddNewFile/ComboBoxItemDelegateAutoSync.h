#ifndef COMBBOXDELEGATEAUTOSYNC_H
#define COMBBOXDELEGATEAUTOSYNC_H

#include <QStyledItemDelegate>

class DialogAddNewFile;

class ComboBoxItemDelegateAutoSync : public QStyledItemDelegate
{
    Q_OBJECT
public:
    ComboBoxItemDelegateAutoSync(DialogAddNewFile *dialog, QObject *parent = nullptr);
    ~ComboBoxItemDelegateAutoSync();


    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

private:
    DialogAddNewFile *dialog;
};


#endif // COMBBOXDELEGATEAUTOSYNC_H
