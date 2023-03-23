#ifndef DIALOG_IMPORT_ITEMDELEGATEACTION_H
#define DIALOG_IMPORT_ITEMDELEGATEACTION_H

namespace TreeModelDialogImport
{
    class ItemDelegateAction;
}

#include "TreeItem.h"

#include <QStyledItemDelegate>

class TreeModelDialogImport::ItemDelegateAction : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ItemDelegateAction(QObject *parent = nullptr);
    ~ItemDelegateAction();

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

signals:
    void refreshChildComboBoxes(TreeItem *parent) const;

private:
    static const QString ITEM_TEXT_IMPORT;
    static const QString ITEM_TEXT_OVERWRITE;
    static const QString ITEM_TEXT_CHOOSE_EACH_CHILDREN;
    static const QString ITEM_TEXT_DO_NOT_IMPORT;
};

#endif // DIALOG_IMPORT_ITEMDELEGATEACTION_H
