#include "TreeItem.h"

TreeItem::TreeItem(TreeItem *parent) : parentItem(parent)
{
    setType(ItemType::Undefined);
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

void TreeItem::setOldName(const QString &newOldName)
{
    oldName = newOldName;
}

int TreeItem::getStatus() const
{
    return status;
}

void TreeItem::setStatus(int newStatus)
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

void TreeItem::setDescription(const QString &newDescription)
{
    description = newDescription;
}

void TreeItem::setAction(const QString &newAction)
{
    action = newAction;
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
