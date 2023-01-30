#include "TreeItem.h"

TreeItem::TreeItem(TreeItem *parent) : parentItem(parent)
{}

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

void TreeItem::setStatus(const QString &newStatus)
{
    status = newStatus;
}

void TreeItem::setOldName(const QString &newOldName)
{
    oldName = newOldName;
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

QVariant TreeItem::data(int column) const
{
    if(column == ColumnIndexUserPath)
        return userPath;
    else if(column == ColumnIndexStatus)
        return status;
    else if(column == ColumnIndexDescription)
        return description;
    else if(column == ColumnIndexAction)
        return action;
    else
        return QVariant();
}

int TreeItem::row() const
{
    if (getParentItem() != nullptr)
        return getParentItem()->childItems.indexOf(const_cast<TreeItem*>(this));

    return 0;
}
