#include "TreeItem.h"

TreeItem::TreeItem(TreeItem *parent) : _parentItem(parent)
{}

TreeItem::~TreeItem()
{
    qDeleteAll(childItems);
}

void TreeItem::setUserPath(const QString &newUserPath)
{
    userPath = newUserPath;
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

TreeItem *TreeItem::parentItem()
{
    return _parentItem;
}

int TreeItem::row() const
{
    if (_parentItem != nullptr)
        return _parentItem->childItems.indexOf(const_cast<TreeItem*>(this));

    return 0;
}
