#include "TableModelNewAddedFiles.h"

#include <QFileIconProvider>
#include <QDir>

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
    return parent.isValid() ? 0 : 4;
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
                if(item.isAutoSyncEnabled)
                    return tr("Enabled");
                else
                    return tr("Disabled");
            case 2:
                return item.location;
            case 3:
            {
                TableItemStatus status = item.status;

                if(status == TableItemStatus::Pending)
                    return tr("Pending");
                else if(status == TableItemStatus::Waiting)
                    return tr("Waiting");
                else if(status == TableItemStatus::Successful)
                    return tr("Successful");
                else if(status == TableItemStatus::Failed)
                    return tr("Failed");
                else
                    return tr("NaN");
            }
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

    if (orientation == Qt::Horizontal)
    {
        switch (section)
        {
            case 0:
                return tr("File Name");
            case 1:
                return tr("Auto-sync");
            case 2:
                return tr("Location");
            case 3:
                return tr("Status");
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
        itemList.insert(position, { QString(), true, TableItemStatus::Waiting, QString() });

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

void TableModelNewAddedFiles::markItemAsPending(const QString &pathToFile)
{
    this->markItemAs(pathToFile, TableItemStatus::Pending);
}

void TableModelNewAddedFiles::markItemAsSuccessful(const QString &pathToFile)
{
    this->markItemAs(pathToFile, TableItemStatus::Successful);
}

void TableModelNewAddedFiles::markItemAsFailed(const QString &pathToFile)
{
    this->markItemAs(pathToFile, TableItemStatus::Failed);
}

void TableModelNewAddedFiles::markItemAs(const QString &pathToFile, TableItemStatus status)
{

    QFileInfo fileInfo(pathToFile);
    auto location = QDir::toNativeSeparators(fileInfo.absolutePath()) + QDir::separator();
    auto fileName = fileInfo.fileName();

    for(TableItem &item : this->itemList)
    {
        if(item.fileName == fileName && item.location == location)
        {
            item.status = status;
            //QModelIndex topLeft = createIndex(0,0);
            //emit dataChanged(topLeft, topLeft);
        }
    }
}

bool TableModelNewAddedFiles::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole)
    {
        const int row = index.row();
        auto item = itemList.value(row);

        switch (index.column())
        {
        case 0:
            item.fileName = value.toString();
            break;
        case 1:
            item.isAutoSyncEnabled = value.toBool();
            break;
        case 2:
            item.location = value.toString();
            break;
        case 3:
            item.status = value.value<TableItemStatus>();
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
