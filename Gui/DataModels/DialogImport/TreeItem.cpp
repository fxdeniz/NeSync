#include "TreeItem.h"

using namespace TreeModelDialogImport;

TreeItem::TreeItem(TreeItem *parentItem)
{
    setParentItem(parentItem);
    setName("");
    setSymbolFolderPath("");
    setType(ItemType::Undefined);
    setAction(Action::NotSelected);
    setStatus(Status::NotSet);
}

TreeItem::~TreeItem()
{
    qDeleteAll(childItems);
}

TreeItem *TreeItem::getParentItem() const
{
    return parentItem;
}

void TreeItem::setParentItem(TreeItem *newParentItem)
{
    parentItem = newParentItem;
}

QString TreeItem::getSymbolFolderPath() const
{
    return symbolFolderPath;
}

void TreeItem::setSymbolFolderPath(const QString &newSymbolFolderPath)
{
    symbolFolderPath = newSymbolFolderPath;
}

QString TreeItem::getName() const
{
    return name;
}

void TreeItem::setName(const QString &newName)
{
    name = newName;
}

TreeItem::ItemType TreeItem::getType() const
{
    return type;
}

void TreeItem::setType(ItemType newType)
{
    type = newType;
}

TreeItem::Action TreeItem::getAction() const
{
    return action;
}

void TreeItem::setAction(Action newAction)
{
    action = newAction;
}

TreeItem::Status TreeItem::getStatus() const
{
    return status;
}

void TreeItem::setStatus(Status newStatus)
{
    status = newStatus;
}

QJsonObject TreeItem::getFileJson() const
{
    return fileJson;
}

void TreeItem::setFileJson(const QJsonObject &newFileJson)
{
    fileJson = newFileJson;
}

void TreeItem::appendChild(TreeItem *item)
{
    childItems.append(item);
}

TreeItem *TreeItem::child(int row)
{
    if (row < 0 || row >= childItems.size())
        return nullptr;
    return childItems.at(row);
}

int TreeItem::childCount() const
{
    return childItems.count();
}

int TreeItem::columnCount() const
{
    return ColumnCount;
}

int TreeItem::row() const
{
    if (getParentItem() != nullptr)
        return getParentItem()->childItems.indexOf(const_cast<TreeItem*>(this));

    return 0;
}
