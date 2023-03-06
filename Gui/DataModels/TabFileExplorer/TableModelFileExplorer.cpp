#include "TableModelFileExplorer.h"

#include "Utility/JsonDtoFormat.h"

#include <QDir>
#include <QColor>
#include <QPixmap>
#include <QJsonArray>
#include <QJsonObject>
#include <QStandardPaths>
#include <QFileIconProvider>

TableModelFileExplorer::TableModelFileExplorer(QJsonObject result, QObject *parent)
    : QAbstractTableModel(parent)
{
    itemList = tableItemListFrom(result);
}

QString TableModelFileExplorer::getNameFromModelIndex(const QModelIndex &index) const
{
    QString result = "";

    if(index.isValid())
        result = itemList.at(index.row()).name;

    return result;}

QString TableModelFileExplorer::getSymbolPathFromModelIndex(const QModelIndex &index) const
{
    QString result = "";

    if(index.isValid())
        result = itemList.at(index.row()).symbolPath;

    return result;
}

QString TableModelFileExplorer::getUserPathFromModelIndex(const QModelIndex &index) const
{
    QString result = "";

    if(index.isValid())
        result = itemList.at(index.row()).userPath;

    return result;
}

bool TableModelFileExplorer::getIsFrozenFromModelIndex(const QModelIndex &index) const
{
    bool result = false;

    if(index.isValid())
        result = itemList.at(index.row()).isFrozen;

    return result;
}

TableModelFileExplorer::TableItemType TableModelFileExplorer::getItemTypeFromModelIndex(const QModelIndex &index) const
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
    return parent.isValid() ? 0 : 5;
}

QVariant TableModelFileExplorer::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= itemList.size() || index.row() < 0)
        return QVariant();

    const TableItem item = itemList.at(index.row());

    if (role == Qt::ItemDataRole::DisplayRole)
    {
        switch (index.column())
        {
            case ColumnIndexName:
                return item.name;
            case ColumnIndexSymbolPath:
                return item.symbolPath;
            case ColumnIndexUserPath:
                return item.userPath;
            case ColumnIndexIsFrozen:
                if(item.isFrozen)
                    return tr("Yes");
                else
                    return tr("No");
            case ColumnIndexItemType:
                return item.type;
            default:
                break;
        }
    }
    else if(role == Qt::ItemDataRole::CheckStateRole && index.column() == ColumnIndexName)
    {
        if(checkedItems.contains(index))
            return Qt::CheckState::Checked;
        else
            return Qt::CheckState::Unchecked;
    }
    else if(role == Qt::ItemDataRole::DecorationRole && index.column() == ColumnIndexName)
    {
        QFileIconProvider provider;

        if(item.type == TableModelFileExplorer::TableItemType::Folder)
            return provider.icon(QFileIconProvider::IconType::Folder);

        else if(item.type == TableModelFileExplorer::TableItemType::File)
        {
            QString iconFilePath = QStandardPaths::writableLocation(QStandardPaths::StandardLocation::TempLocation);
            iconFilePath = QDir::toNativeSeparators(iconFilePath) + QDir::separator();
            iconFilePath.append(item.name);

            QFileInfo info(iconFilePath);

            return provider.icon(info);
        }
    }
    else if (role == Qt::ItemDataRole::BackgroundRole)
    {
        if(item.isFrozen)
        {
            if(index.column() == ColumnIndexIsFrozen)
                return QColor(Qt::GlobalColor::darkGray);

            return QColor(Qt::GlobalColor::lightGray);
        }
    }
    else if(role == Qt::ItemDataRole::TextAlignmentRole && index.column() == ColumnIndexIsFrozen)
    {
        return Qt::AlignmentFlag::AlignCenter;
    }

    return QVariant();
}

QVariant TableModelFileExplorer::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
    {
        switch (section)
        {
            case ColumnIndexName:
                return tr("Name");
            case ColumnIndexSymbolPath:
                return tr("Symbol Path");
            case ColumnIndexUserPath:
                return tr("Located at");
            case ColumnIndexIsFrozen:
                return tr("Frozen");
            case ColumnIndexItemType:
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
    QJsonArray childFolders = parentFolder[JsonKeys::Folder::ChildFolders].toArray();

    for(const QJsonValue &jsonValue : childFolders)
    {
        QJsonObject child = jsonValue.toObject();
        TableItem item {
                        child[JsonKeys::Folder::SuffixPath].toString().chopped(1), // Remove / character at end
                        child[JsonKeys::Folder::SymbolFolderPath].toString(),
                        child[JsonKeys::Folder::UserFolderPath].toString(),
                        child[JsonKeys::Folder::IsFrozen].toBool(),
                        TableItemType::Folder,
                       };

        result.append(item);
    }

    QJsonArray childFiles = parentFolder[JsonKeys::Folder::ChildFiles].toArray();

    for(const QJsonValue &jsonValue : childFiles)
    {
        QJsonObject child = jsonValue.toObject();

        TableItem item {
                        child[JsonKeys::File::FileName].toString(),
                        child[JsonKeys::File::SymbolFilePath].toString(),
                        child[JsonKeys::File::UserFilePath].toString(),
                        child[JsonKeys::File::IsFrozen].toBool(),
                        TableItemType::File,
                       };

        result.append(item);
    }

    return result;
}
