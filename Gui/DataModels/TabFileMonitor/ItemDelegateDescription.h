#ifndef ITEMDELEGATEDESCRIPTION_H
#define ITEMDELEGATEDESCRIPTION_H

#include <QStyledItemDelegate>

class ItemDelegateDescription : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ItemDelegateDescription(QObject *parent = nullptr);
    ~ItemDelegateDescription();

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

signals:

};

#endif // ITEMDELEGATEDESCRIPTION_H
