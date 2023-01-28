#include "TableModelFileExplorer.h"

#include "Utility/JsonDtoFormat.h"

#include <QColor>
#include <QPixmap>
#include <QJsonArray>
#include <QJsonObject>
#include <QFileIconProvider>

TableModelFileExplorer::TableModelFileExplorer(QJsonObject result, QObject *parent)
    : QAbstractTableModel(parent)
{
    itemList = tableItemListFrom(result);
}

QString TableModelFileExplorer::symbolPathFromModelIndex(const QModelIndex &index) const
{
    QString result = "";

    if(index.isValid())
        result = itemList.at(index.row()).symbolPath;

    return result;
}

TableModelFileExplorer::TableItemType TableModelFileExplorer::itemTypeFromModelIndex(const QModelIndex &index) const
{
    TableItemType result = TableItemType::Invalid;

    if(index.isValid())
        result = itemList.at(index.row()).type;

    return result;
}

int TableModelFileExplorer::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : this->itemList.size();
}

int TableModelFileExplorer::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : 3;
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
                return item.symbolPath;
            case 2:
                return item.type;
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
        const auto &item = this->itemList.at(index.row());

        return item.icon;
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
            return tr("Name");
        case 1:
            return tr("Symbol Path");
        case 2:
            return tr("Type");
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
                item.symbolPath = value.toString();
                break;
            case 2:
                item.type = value.value<TableItemType>();
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

QList<TableModelFileExplorer::TableItem> TableModelFileExplorer::tableItemListFrom(QJsonObject parentFolder)
{
    QList<TableItem> result;
    QFileIconProvider iconProvider;
    QJsonArray childFolders = parentFolder[JsonKeys::Folder::ChildFolders].toArray();

    for(const QJsonValue &jsonValue : childFolders)
    {
        QJsonObject child = jsonValue.toObject();
        TableItem item {
                        child[JsonKeys::Folder::SuffixPath].toString().chopped(1), // Remove / character at end
                        child[JsonKeys::Folder::SymbolFolderPath].toString(),
                        TableItemType::Folder,
                        iconProvider.icon(QFileIconProvider::IconType::Folder)
                       };

        result.append(item);
    }

    QJsonArray childFiles = parentFolder[JsonKeys::Folder::ChildFiles].toArray();

    for(const QJsonValue &jsonValue : childFiles)
    {
        QJsonObject child = jsonValue.toObject();
        QFileInfo info(child[JsonKeys::File::UserFilePath].toString());
        TableItem item {
                        child[JsonKeys::File::FileName].toString(),
                        child[JsonKeys::File::SymbolFilePath].toString(),
                        TableItemType::File,
                        iconProvider.icon(info)
                       };

        result.append(item);
    }

    return result;
}
