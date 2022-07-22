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

TableModelFileMonitor::TableItem TableModelFileMonitor::tableItemNewAddedFolderFrom(const QString &pathToFolder)
{
    return TableModelFileMonitor::tableItemFolderFrom(pathToFolder, TableItemStatus::NewAdded);
}

TableModelFileMonitor::TableItem TableModelFileMonitor::tableItemModifiedFolderFrom(const QString &pathToFolder)
{
    return TableModelFileMonitor::tableItemFolderFrom(pathToFolder, TableItemStatus::Modified);
}

TableModelFileMonitor::TableItem TableModelFileMonitor::tableItemDeletedFolderFrom(const QString &pathToFolder)
{
    return TableModelFileMonitor::tableItemFolderFrom(pathToFolder, TableItemStatus::Deleted);
}

TableModelFileMonitor::TableItem TableModelFileMonitor::tableItemMovedFolderFrom(const QString &pathToFolder,
                                                                                 const QString &oldLocation)
{
    return TableModelFileMonitor::tableItemFolderFrom(pathToFolder, TableItemStatus::Moved, oldLocation);
}

TableModelFileMonitor::TableItem TableModelFileMonitor::tableItemNewAddedFileFrom(const QString &pathToFile)
{
    return TableModelFileMonitor::tableItemFileFrom(pathToFile, TableItemStatus::NewAdded);
}

TableModelFileMonitor::TableItem TableModelFileMonitor::tableItemDeletedFileFrom(const QString &pathToFile)
{
    return TableModelFileMonitor::tableItemFileFrom(pathToFile, TableItemStatus::Deleted);
}

TableModelFileMonitor::TableItem TableModelFileMonitor::tableItemMovedFileFrom(const QString &pathToFile,
                                                                               const QString &oldLocation)
{
    return TableModelFileMonitor::tableItemFileFrom(pathToFile, TableItemStatus::Moved, oldLocation);
}

TableModelFileMonitor::TableItem TableModelFileMonitor::tableItemModifiedFileFrom(const QString &pathToFile)
{
    return TableModelFileMonitor::tableItemFileFrom(pathToFile, TableItemStatus::Modified);
}

TableModelFileMonitor::TableItem TableModelFileMonitor::tableItemMovedAndModifiedFileFrom(const QString &pathToFile,
                                                                                         const QString &oldLocation)
{
    return TableModelFileMonitor::tableItemFileFrom(pathToFile, TableItemStatus::MovedAndModified, oldLocation);
}

int TableModelFileMonitor::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : this->itemList.size();
}

int TableModelFileMonitor::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : 8;
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
                return item.name;
            case 1:
                if(item.itemType == TableItemType::File)
                    return item.parentDirPath;
                else if(item.itemType == TableItemType::Folder)
                {
                    QFileInfo info(item.parentDirPath);
                    QDir dir = info.dir();
                    dir.cdUp();
                    QString parentDir = QDir::toNativeSeparators(dir.absolutePath()) + QDir::separator();
                    return parentDir;
                }
                else
                    return "NaN";
            case 2:
                if(item.itemType == TableItemType::File)
                    return item.oldName;
                else if (item.itemType == TableItemType::Folder && !item.oldName.isEmpty())
                    return item.oldName + QDir::separator();
                else
                    return "";
            case 3:
                if(item.itemType == TableItemType::Folder)
                    return tr("Folder");
                else if(item.itemType == TableItemType::File)
                    return tr("File");
                else
                    return "NaN";
            case 4:
                if(item.status == TableItemStatus::Modified)
                    return tr("Updated");
                else if(item.status == TableItemStatus::NewAdded)
                    return tr("New Added");
                else if(item.status == TableItemStatus::Deleted)
                    return tr("Deleted");
                else if(item.status == TableItemStatus::Moved)
                    return tr("Moved");
                else if(item.status == TableItemStatus::MovedAndModified)
                    return tr("Moved & Updated");
                else if(item.status == TableItemStatus::Missing)
                    return tr("Missing");
                else if(item.status == TableItemStatus::InvalidStatus)
                    return tr("Invalid");
                else
                    return tr("NaN");
            case 5:
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
                QFileInfo info(item.name);
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
        if(index.column() == 2 || index.column() == 3 || index.column() == 4 || index.column() == 5)
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
                return tr("Old Name");
            case 3:
                return tr("Type");
            case 4:
                return tr("Status");
            case 5:
                return tr("Timestamp");
            case 6:
                return tr("Action");
            case 7:
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
                item.name = value.toString();
                break;
            case 1:
                item.parentDirPath = value.toString();
                break;
            case 2:
                item.oldName = value.toString();
                break;
            case 3:
                item.itemType = value.value<TableItemType>();
                break;
            case 4:
                item.status = value.value<TableItemStatus>();
                break;
            case 5:
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

TableModelFileMonitor::TableItem TableModelFileMonitor::tableItemFolderFrom(const QString &pathToFolder,
                                                                            TableItemStatus status,
                                                                            const QString &oldLocation)
{
    QDir dir(pathToFolder);
    QString dirName = dir.dirName() + QDir::separator();
    QString parentDirPath = QDir::toNativeSeparators(dir.absolutePath()) + QDir::separator();

    TableModelFileMonitor::TableItem result {
        dirName,
        parentDirPath,
        oldLocation,
        TableModelFileMonitor::TableItemType::Folder,
        status,
        QDateTime::currentDateTime()
    };

    return result;
}

TableModelFileMonitor::TableItem TableModelFileMonitor::tableItemFileFrom(const QString &pathToFile,
                                                                          TableItemStatus status,
                                                                          const QString &oldLocation)
{
    QFileInfo fileInfo(pathToFile);
    auto fileDir = QDir::toNativeSeparators(fileInfo.absolutePath()) + QDir::separator();
    TableModelFileMonitor::TableItem result {
        fileInfo.fileName(),
        fileDir,
        oldLocation,
        TableModelFileMonitor::TableItemType::File,
        status,
        QDateTime::currentDateTime()
    };

    return result;
}
