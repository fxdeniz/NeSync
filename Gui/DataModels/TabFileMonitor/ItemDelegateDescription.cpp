#include "ItemDelegateDescription.h"

#include "TreeItem.h"

#include "TreeModelFsMonitor.h"

#include <QComboBox>
#include <QStringListModel>

const QString ItemDelegateDescription::ITEM_TEXT_DEFAULT = tr("Not selected");

ItemDelegateDescription::ItemDelegateDescription(QObject *parent)
    : QStyledItemDelegate(parent)
{

}

ItemDelegateDescription::~ItemDelegateDescription()
{

}

QWidget *ItemDelegateDescription::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QComboBox *result = nullptr;
    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    if(item->getType() == TreeItem::ItemType::File)
    {
        result = new QComboBox(parent);

        const TreeModelFsMonitor *treeModel = qobject_cast<const TreeModelFsMonitor *>(index.model());
        Q_ASSERT(treeModel);

        result->setModel(treeModel->getDescriptionNumberListModel());
        result->setCurrentIndex(-1);
        result->setPlaceholderText(ITEM_TEXT_DEFAULT);

        QObject::connect(treeModel->getDescriptionNumberListModel(), &QAbstractListModel::modelAboutToBeReset,
                         this, [=]{
            result->setProperty("prevDescIndex", result->currentIndex());
            result->setProperty("prevDescNumber", result->currentText().toInt());
        });

        QObject::connect(treeModel->getDescriptionNumberListModel(), &QAbstractListModel::modelReset,
                         this, [=]{
            int prevDescIndex = result->property("prevDescIndex").toInt();
            int prevDescNumber = result->property("prevDescNumber").toInt();
            bool isPreviousDescExist = treeModel->isDescriptionExist(prevDescNumber);

            if(isPreviousDescExist)
                result->setCurrentIndex(prevDescIndex); // Preserve the previous selection
            else
                result->setCurrentIndex(-1);

        });
    }

    return result;
}

void ItemDelegateDescription::setEditorData(QWidget *editor, const QModelIndex &index) const
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

void ItemDelegateDescription::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *cb = qobject_cast<QComboBox *>(editor);
    Q_ASSERT(cb);

    TreeItem *item = static_cast<TreeItem *>(index.internalPointer());

    TreeItem::Action action;

//    if(cb->currentText() == ITEM_TEXT_SAVE)
//        action = TreeItem::Action::Save;

//    else if(cb->currentText() == ITEM_TEXT_RESTORE)
//        action = TreeItem::Action::Restore;

//    else if(cb->currentText() == ITEM_TEXT_FREEZE)
//        action = TreeItem::Action::Freeze;

//    else if(cb->currentText() == ITEM_TEXT_DELETE)
//        action = TreeItem::Action::Delete;

//    else
//        action = TreeItem::Action::NotSelected;

    item->setAction(action);
}
