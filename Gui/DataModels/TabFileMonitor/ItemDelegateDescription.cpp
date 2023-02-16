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

    // Only create ItemDelegateDescription for files
    //      and do not create ItemDelegateDescription for deleted files
    if(item->getType() == TreeItem::ItemType::File)
    {
        if(item->getStatus() != FileSystemEventDb::ItemStatus::Deleted &&
           item->getStatus() != FileSystemEventDb::ItemStatus::Missing)
        {
            result = new QComboBox(parent);

            TreeModelFsMonitor *treeModel = (TreeModelFsMonitor *) index.model();
            Q_ASSERT(treeModel);

            result->setModel(treeModel->getDescriptionNumberListModel());
            result->setCurrentIndex(-1);
            result->setPlaceholderText(ITEM_TEXT_DEFAULT);

            // This lambda connection causes immediate trigger of ItemDelegateDescription::setModelData()
            QObject::connect(result, &QComboBox::textActivated,
                             this, [=](const QString &item){
                Q_UNUSED(item);
                result->clearFocus();
            });

            QObject::connect(treeModel, &TreeModelFsMonitor::signalDisableItemDelegates,
                             result, [=]{
                    result->setDisabled(true);
                    if(result->currentText().isEmpty())
                        item->setDescription("");
                    else
                    {
                        int number = result->currentText().toInt();
                        item->setDescription(treeModel->getDescription(number));
                    }
            });

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
                {
                    result->setCurrentIndex(-1);
                    item->setDescription("");
                }

            });
        }
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
    QComboBox *comboBox = qobject_cast<QComboBox *>(editor);
    Q_ASSERT(comboBox);

    TreeItem *item = static_cast<TreeItem *>(index.internalPointer());

    QString currentText = comboBox->currentText();
    if(!currentText.isEmpty())
    {
        const TreeModelFsMonitor *treeModel = qobject_cast<const TreeModelFsMonitor *>(index.model());

        int number = currentText.toInt();
        item->setDescription(treeModel->getDescription(number));
    }
}
