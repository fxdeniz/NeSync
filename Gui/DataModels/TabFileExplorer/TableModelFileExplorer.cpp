#include "TableModelFileExplorer.h"

#include <QFileIconProvider>
#include <QPixmap>
#include <QColor>

TableModelFileExplorer::TableModelFileExplorer(QObject *parent)
    : QAbstractTableModel(parent)
{
}

TableModelFileExplorer::TableModelFileExplorer(const QList<TableItem> &_itemList, QObject *parent)
    : QAbstractTableModel(parent), itemList(_itemList)
{
}

int TableModelFileExplorer::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : this->itemList.size();
}

int TableModelFileExplorer::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : 2;
}

QVariant TableModelFileExplorer::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= this->itemList.size() || index.row() < 0)
        return QVariant();

    if (role == Qt::ItemDataRole::DisplayRole)
    {
        const auto &item = this->itemList.at(index.row());

        switch (index.column())
        {
            case 0:
                return item.name;
            case 1:
                return item.itemCount;
            default:
                break;
        }
    }
    else if(role == Qt::ItemDataRole::CheckStateRole && index.column() == 0)
    {
        if(this->checkedItems.contains(index))
            return Qt::CheckState::Checked;
        else
            return Qt::CheckState::Unchecked;
    }
    else if(role == Qt::ItemDataRole::DecorationRole && index.column() == 0)
    {
        QFileIconProvider provider;
        auto result = provider.icon(QFileIconProvider::IconType::Folder).pixmap(20, 20);

        return result;
    }

    return QVariant();
}

QVariant TableModelFileExplorer::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
            return tr("File Name");
        case 1:
            return tr("Extension");
        default:
            break;
        }
    }
    return QVariant();
}

Qt::ItemFlags TableModelFileExplorer::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractTableModel::flags(index) | Qt::ItemFlag::ItemIsEditable | Qt::ItemFlag::ItemIsUserCheckable;
}

bool TableModelFileExplorer::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid())
    {
        if(role == Qt::ItemDataRole::EditRole)
        {
            const int row = index.row();
            auto item = this->itemList.value(row);

            switch (index.column())
            {
            case 0:
                item.name = value.toString();
                break;
            case 1:
                item.itemCount = value.toString();
                break;
            default:
                return false;
            }
            this->itemList.replace(row, item);
            emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});

            return true;
        }
        else if(role == Qt::ItemDataRole::CheckStateRole)
        {
            if(value == Qt::CheckState::Checked)
                this->checkedItems.insert(index);
            else
                this->checkedItems.remove(index);

            emit dataChanged(index, index);
            return true;
        }
    }

    return false;
}

bool TableModelFileExplorer::insertRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);
    beginInsertRows(QModelIndex(), position, position + rows - 1);

    for (int row = 0; row < rows; ++row)
        this->itemList.insert(position, { QString(), QString() });

    endInsertRows();
    return true;
}

bool TableModelFileExplorer::removeRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);
    beginRemoveRows(QModelIndex(), position, position + rows - 1);

    for (int row = 0; row < rows; ++row)
        this->itemList.removeAt(position);

    endRemoveRows();
    return true;
}
