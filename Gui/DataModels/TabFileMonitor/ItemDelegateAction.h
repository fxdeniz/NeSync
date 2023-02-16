#ifndef ITEMDELEGATEACTION_H
#define ITEMDELEGATEACTION_H


#include <QStyledItemDelegate>

class ItemDelegateAction : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ItemDelegateAction(QObject *parent = nullptr);
    ~ItemDelegateAction();

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

private:
    static const QString ITEM_TEXT_SAVE;
    static const QString ITEM_TEXT_RESTORE;
    static const QString ITEM_TEXT_FREEZE;
    static const QString ITEM_TEXT_DELETE;

};

#endif // ITEMDELEGATEACTION_H
