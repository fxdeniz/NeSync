#include "TableModelFileMonitor.h"

#include <QFileIconProvider>
#include <QPixmap>
#include <QColor>
#include <QDir>

TableModelFileMonitor::TableModelFileMonitor(QObject *parent)
    : QAbstractTableModel(parent)
{
}

TableModelFileMonitor::TableModelFileMonitor(const QList<TableItem> &_itemList, QObject *parent)
    : QAbstractTableModel(parent), itemList(_itemList)
{
}

int TableModelFileMonitor::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : this->itemList.size();
}

int TableModelFileMonitor::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : 7;
}

QVariant TableModelFileMonitor::data(const QModelIndex &index, int role) const
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
                if(item.itemType == TableItemType::File)
                    return item.fileName;
                else if(item.itemType == TableItemType::Folder)
                {
                    QFileInfo info(item.folderPath);
                    return info.absoluteDir().dirName() + QDir::separator();
                }
                else
                    return "NaN";
            case 1:
                if(item.itemType == TableItemType::File)
                    return item.folderPath;
                else if(item.itemType == TableItemType::Folder)
                {
                    QFileInfo info(item.folderPath);
                    QDir dir = info.dir();
                    dir.cdUp();
                    QString parentDir = QDir::toNativeSeparators(dir.absolutePath()) + QDir::separator();
                    return parentDir;
                }
                else
                    return "NaN";
            case 2:
                if(item.itemType == TableItemType::Folder)
                    return tr("Folder");
                else if(item.itemType == TableItemType::File)
                    return tr("File");
                else
                    return "NaN";
            case 3:
                if(item.eventType == TableItemStatus::Updated)
                    return tr("Updated");
                else if(item.eventType == TableItemStatus::NewAdded)
                    return tr("New Added");
                else if(item.eventType == TableItemStatus::Deleted)
                    return tr("Deleted");
                else if(item.eventType == TableItemStatus::Moved)
                    return tr("Moved");
                else if(item.eventType == TableItemStatus::Missing)
                    return tr("Missing");
                else if(item.eventType == TableItemStatus::Invalid)
                    return tr("Invalid");
                else
                    return tr("NaN");
            case 4:
                return item.timestamp;
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
    else if(role == Qt::ItemDataRole::DecorationRole)
    {
        if(index.column() == 0)
        {
            const auto &item = this->itemList.at(index.row());

            if(item.itemType == TableItemType::File)
            {
                QFileIconProvider provider;
                QFileInfo info(item.fileName);
                auto result = provider.icon(info);

                return result;
            }
            else if(item.itemType == TableItemType::Folder)
            {
                QFileIconProvider provider;
                auto result = provider.icon(QFileIconProvider::IconType::Folder);

                return result;
            }
        }
        else if(index.column() == 1)
        {
            QFileIconProvider provider;
            auto result = provider.icon(QFileIconProvider::IconType::Folder).pixmap(32, 32);

            return result;
        }
    }
    else if(role == Qt::ItemDataRole::TextAlignmentRole)
    {
        if(index.column() == 2 || index.column() == 3 || index.column() == 4)
            return Qt::AlignmentFlag::AlignCenter;
    }

    return QVariant();
}

QVariant TableModelFileMonitor::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
    {
        switch (section)
        {
            case 0:
                return tr("Name");
            case 1:
                return tr("Location");
            case 2:
                return tr("Type");
            case 3:
                return tr("Event");
            case 4:
                return tr("Timestamp");
            case 5:
                return tr("Action");
            case 6:
                return tr("Note");
            default:
                break;
        }
    }
    return QVariant();
}

Qt::ItemFlags TableModelFileMonitor::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractTableModel::flags(index) | Qt::ItemFlag::ItemIsEditable | Qt::ItemFlag::ItemIsUserCheckable;
}

bool TableModelFileMonitor::setData(const QModelIndex &index, const QVariant &value, int role)
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
                item.fileName = value.toString();
                break;
            case 1:
                item.folderPath = value.toString();
                break;
            case 2:
                item.itemType = value.value<TableItemType>();
                break;
            case 3:
                item.eventType = value.value<TableItemStatus>();
                break;
            case 4:
                item.timestamp = value.toDateTime();
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

bool TableModelFileMonitor::insertRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);
    beginInsertRows(QModelIndex(), position, position + rows - 1);

    for (int row = 0; row < rows; ++row)
        this->itemList.insert(position, { QString(), QString() });

    endInsertRows();

    return true;
}

bool TableModelFileMonitor::removeRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);
    beginRemoveRows(QModelIndex(), position, position + rows - 1);

    for (int row = 0; row < rows; ++row)
        this->itemList.removeAt(position);

    endRemoveRows();
    return true;
}

