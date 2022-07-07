#include "TableModelNewAddedFiles.h"

#include <QFileIconProvider>

TableModelNewAddedFiles::TableModelNewAddedFiles(QObject *parent)
    : QAbstractTableModel(parent)
{
}

TableModelNewAddedFiles::TableModelNewAddedFiles(const QList<TableItem> &contacts, QObject *parent)
    : QAbstractTableModel(parent), itemList(contacts)
{
}

int TableModelNewAddedFiles::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : itemList.size();
}

int TableModelNewAddedFiles::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : 2;
}

QVariant TableModelNewAddedFiles::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= itemList.size() || index.row() < 0)
        return QVariant();

    if (role == Qt::DisplayRole)
    {
        const auto &item = itemList.at(index.row());

        switch (index.column())
        {
            case 0:
                return item.fileName;
            case 1:
                return item.location;
            default:
                break;
        }
    }
    else if(role == Qt::ItemDataRole::DecorationRole && index.column() == 0)
    {
        const auto &item = itemList.at(index.row());

        QFileIconProvider provider;
        QFileInfo fileInfo(item.location + item.fileName);
        auto result = provider.icon(fileInfo).pixmap(16, 16);

        return result;
    }

    return QVariant();
}

QVariant TableModelNewAddedFiles::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
            return tr("File Name");
        case 1:
            return tr("Location");
        default:
            break;
        }
    }
    return QVariant();
}

bool TableModelNewAddedFiles::insertRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);
    beginInsertRows(QModelIndex(), position, position + rows - 1);

    for (int row = 0; row < rows; ++row)
        itemList.insert(position, { QString(), QString() });

    endInsertRows();
    return true;
}

bool TableModelNewAddedFiles::removeRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);
    beginRemoveRows(QModelIndex(), position, position + rows - 1);

    for (int row = 0; row < rows; ++row)
        itemList.removeAt(position);

    endRemoveRows();
    return true;
}

const QList<TableModelNewAddedFiles::TableItem> &TableModelNewAddedFiles::getItemList() const
{
    return itemList;
}

QStringList TableModelNewAddedFiles::getFilePathList() const
{
    QStringList result;

    for(const TableItem &item : this->itemList)
        result.append(item.location + item.fileName);

    return result;
}

bool TableModelNewAddedFiles::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
        const int row = index.row();
        auto item = itemList.value(row);

        switch (index.column()) {
        case 0:
            item.fileName = value.toString();
            break;
        case 1:
            item.location = value.toString();
            break;
        default:
            return false;
        }
        itemList.replace(row, item);
        emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});

        return true;
    }

    return false;
}

Qt::ItemFlags TableModelNewAddedFiles::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}
