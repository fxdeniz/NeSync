#include "TreeItem.h"

TreeItem::TreeItem(TreeItem *parent) : parentItem(parent)
{
    setType(ItemType::Undefined);
    setAction(TreeItem::Action::NotSelected);
    setDescription("");
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

void TreeItem::setUserPath(const QString &newUserPath)
{
    userPath = newUserPath;
}


QString TreeItem::getUserPath() const
{
    return userPath;
}

FileSystemEventDb::ItemStatus TreeItem::getStatus() const
{
    return status;
}

void TreeItem::setStatus(FileSystemEventDb::ItemStatus newStatus)
{
    status = newStatus;
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

QString TreeItem::getDescription() const
{
    return description;
}

void TreeItem::setDescription(const QString &newDescription)
{
    description = newDescription;
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
    return 4;
}


int TreeItem::row() const
{
    if (getParentItem() != nullptr)
        return getParentItem()->childItems.indexOf(const_cast<TreeItem*>(this));

    return 0;
}
