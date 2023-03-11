#ifndef FILE_MONITOR_ITEMDELEGATEACTION_H
#define FILE_MONITOR_ITEMDELEGATEACTION_H


#include <QStyledItemDelegate>

namespace TreeModelFileMonitor
{
    class ItemDelegateAction;
}

class TreeModelFileMonitor::ItemDelegateAction : public QStyledItemDelegate
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

    static const QString ITEM_TEXT_SAVE_WITH_CHILDREN;
    static const QString ITEM_TEXT_RESTORE_WITH_CHILDREN;
    static const QString ITEM_TEXT_FREEZE_WITH_CHILDREN;
    static const QString ITEM_TEXT_DELETE_WITH_CHILDREN;
};

#endif // FILE_MONITOR_ITEMDELEGATEACTION_H
