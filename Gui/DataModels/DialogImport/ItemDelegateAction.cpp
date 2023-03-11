#include "ItemDelegateAction.h"
#include "TreeItem.h"

#include <QComboBox>

using namespace TreeModelDialogImport;

const QString ItemDelegateAction::ITEM_TEXT_IMPORT = tr("Import");
const QString ItemDelegateAction::ITEM_TEXT_OVERWRITE = tr("Overwrite");
const QString ItemDelegateAction::ITEM_TEXT_CHOOSE_EACH_CHILDREN = tr("Choose each children seperately");
const QString ItemDelegateAction::ITEM_TEXT_DO_NOT_IMPORT = tr("Do not import");

ItemDelegateAction::ItemDelegateAction(QObject *parent) : QStyledItemDelegate {parent}
{

}

ItemDelegateAction::~ItemDelegateAction()
{

}

QWidget *ItemDelegateAction::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QComboBox *result = new QComboBox(parent);
    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    if(item->getType() == TreeItem::ItemType::Folder)
    {
        if(item->getStatus() == TreeItem::Status::NewFolder)
        {
            item->setAction(TreeItem::Action::Import);
            result->addItem(ITEM_TEXT_IMPORT);
        }
        else if(item->getStatus() == TreeItem::Status::ExistingFolder)
        {
            item->setAction(TreeItem::Action::FollowChildren);
            result->addItem(ITEM_TEXT_CHOOSE_EACH_CHILDREN);
        }

        result->addItem(ITEM_TEXT_DO_NOT_IMPORT);
    }
    else if(item->getType() == TreeItem::ItemType::File)
    {
        if(item->getStatus() == TreeItem::Status::NewFile)
        {
            item->setAction(TreeItem::Action::Import);
            result->addItem(ITEM_TEXT_IMPORT);
        }
        else if(item->getStatus() == TreeItem::Status::ExistingFile)
        {
            item->setAction(TreeItem::Action::Overwrite);
            result->addItem(ITEM_TEXT_OVERWRITE);
        }

        result->addItem(ITEM_TEXT_DO_NOT_IMPORT);
    }
    else
    {
        delete result;
        result = nullptr;
    }

    if(result != nullptr)
    {
        // This lambda connection causes immediate trigger of ItemDelegateDescription::setModelData()
        QObject::connect(result, &QComboBox::textActivated,
                         this, [=](const QString &item){
            Q_UNUSED(item);
            result->clearFocus();
        });
    }

    return result;
}

void ItemDelegateAction::setEditorData(QWidget *editor, const QModelIndex &index) const
{

}

void ItemDelegateAction::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *comboBox = qobject_cast<QComboBox *>(editor);
    Q_ASSERT(comboBox);

    TreeItem *item = static_cast<TreeItem *>(index.internalPointer());
    TreeItem::Action action;
    QString currentText = comboBox->currentText();
}
