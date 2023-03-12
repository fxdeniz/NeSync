#include "ItemDelegateAction.h"


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
        item->setAction(TreeItem::Action::ChooseEachChildren);

        result->addItem(ITEM_TEXT_CHOOSE_EACH_CHILDREN);
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

        QObject::connect(this, &ItemDelegateAction::refreshChildComboBoxes, this, [=](TreeItem *parent){
            if(item->getParentItem() != parent)
                return;

            QString currentText = "";

            if(item->getParentItem()->getAction() == TreeItem::Action::DoNotImport)
            {
                currentText = ITEM_TEXT_DO_NOT_IMPORT;
                result->setDisabled(true);
            }
            else if(item->getParentItem()->getAction() == TreeItem::Action::ChooseEachChildren)
            {
                if(item->getStatus() == TreeItem::Status::ExistingFile)
                    currentText = ITEM_TEXT_OVERWRITE;
                else if(item->getStatus() == TreeItem::Status::NewFile)
                    currentText = ITEM_TEXT_IMPORT;

                result->setEnabled(true);
            }

            const int cbIndex = result->findText(currentText);
            // if it is valid, adjust the combobox
            if (cbIndex >= 0)
                result->setCurrentIndex(cbIndex);
        });
    }

    return result;
}

void ItemDelegateAction::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *comboBox = qobject_cast<QComboBox *>(editor);
    Q_ASSERT(comboBox);

    TreeItem *item = static_cast<TreeItem *>(index.internalPointer());
    TreeItem::Action action;
    QString currentText = comboBox->currentText();

    if(currentText == ITEM_TEXT_IMPORT)
        item->setAction(TreeItem::Action::Import);

    else if(currentText == ITEM_TEXT_OVERWRITE)
        item->setAction(TreeItem::Action::Overwrite);

    else if(currentText == ITEM_TEXT_CHOOSE_EACH_CHILDREN)
        item->setAction(TreeItem::Action::ChooseEachChildren);

    else if(currentText == ITEM_TEXT_DO_NOT_IMPORT)
        item->setAction(TreeItem::Action::DoNotImport);

    if(item->getType() == TreeItem::ItemType::Folder)
    {
        if(currentText == ITEM_TEXT_DO_NOT_IMPORT)
        {
            for(int index = 0; index < item->childCount(); index ++)
            {
                TreeItem *child = item->child(index);
                child->setAction(TreeItem::DoNotImport);
            }
        }

        emit refreshChildComboBoxes(item);
    }
}
