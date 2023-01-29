#include "TreeItem.h"

TreeItem::TreeItem(const QList<QVariant> &data, TreeItem *parent)
    : itemData(data), _parentItem(parent)
{}

TreeItem::~TreeItem()
{
    qDeleteAll(childItems);
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
    return itemData.count();
}

QVariant TreeItem::data(int column) const
{
    if (column < 0 || column >= itemData.size())
        return QVariant();
    return itemData.at(column);
}

TreeItem *TreeItem::parentItem()
{
    return _parentItem;
}

int TreeItem::row() const
{
    if (_parentItem)
        return _parentItem->childItems.indexOf(const_cast<TreeItem*>(this));

    return 0;
}
