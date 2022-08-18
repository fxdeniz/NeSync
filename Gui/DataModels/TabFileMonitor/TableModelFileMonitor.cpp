#include "TableModelFileMonitor.h"

#include <QColor>
#include <QDateTime>
#include <QFileIconProvider>

const QString TableModelFileMonitor::TABLE_NAME = "TableItem";
const QString TableModelFileMonitor::COLUMN_NAME_NAME = "name";
const QString TableModelFileMonitor::COLUMN_NAME_PARENT_DIR = "parent_dir";
const QString TableModelFileMonitor::COLUMN_NAME_PATH = "path";
const QString TableModelFileMonitor::COLUMN_NAME_OLD_NAME = "old_name";
const QString TableModelFileMonitor::COLUMN_NAME_TYPE = "type";
const QString TableModelFileMonitor::COLUMN_NAME_STATUS = "status";
const QString TableModelFileMonitor::COLUMN_NAME_TIMESTAMP = "timestamp";
const QString TableModelFileMonitor::COLUMN_NAME_AUTOSYNC_STATUS = "auto_sync_status";
const QString TableModelFileMonitor::COLUMN_NAME_PROGRESS = "progress";
const QString TableModelFileMonitor::COLUMN_NAME_CURRENT_VERSION = "current_version";
const QString TableModelFileMonitor::COLUMN_NAME_ACTION = "action";
const QString TableModelFileMonitor::COLUMN_NAME_NOTE_NUMBER = "note_number";

const QString TableModelFileMonitor::STATUS_TEXT_MODIFIED = tr("Updated");
const QString TableModelFileMonitor::STATUS_TEXT_NEW_ADDED = tr("New Added");
const QString TableModelFileMonitor::STATUS_TEXT_DELETED = tr("Deleted");
const QString TableModelFileMonitor::STATUS_TEXT_MOVED = tr("Moved");
const QString TableModelFileMonitor::STATUS_TEXT_MOVED_AND_MODIFIED = tr("Moved & Updated");
const QString TableModelFileMonitor::STATUS_TEXT_MISSING = tr("Missing");
const QString TableModelFileMonitor::STATUS_TEXT_INVALID = tr("Invalid");

const QString TableModelFileMonitor::AUTO_SYNC_STATUS_ENABLED_TEXT = tr("Enabled");
const QString TableModelFileMonitor::AUTO_SYNC_STATUS_DISABLED_TEXT = tr("Disabled");


TableModelFileMonitor::TableModelFileMonitor(QObject *parent)
    : QSqlQueryModel(parent)
{
}

TableModelFileMonitor::ItemStatus TableModelFileMonitor::statusCodeFromString(const QString &status)
{
    if(status == STATUS_TEXT_MODIFIED)
        return ItemStatus::Modified;

    else if(status == STATUS_TEXT_NEW_ADDED)
        return ItemStatus::NewAdded;

    else if(status == STATUS_TEXT_DELETED)
        return ItemStatus::Deleted;

    else if(status == STATUS_TEXT_MOVED)
        return ItemStatus::Moved;

    else if(status == STATUS_TEXT_MOVED_AND_MODIFIED)
        return ItemStatus::MovedAndModified;

    else if(status == STATUS_TEXT_MISSING)
        return ItemStatus::Missing;

    else
        return ItemStatus::InvalidStatus;
}

QVariant TableModelFileMonitor::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
    {
        switch (section)
        {
        case ColumnIndex::Name:
            return tr("Name");
        case ColumnIndex::ParentDir:
            return tr("Location");
        case ColumnIndex::Path:
            return tr("Path");
        case ColumnIndex::OldName:
            return tr("Old Name");
        case ColumnIndex::Type:
            return tr("Type");
        case ColumnIndex::Status:
            return tr("Status");
        case ColumnIndex::Timestamp:
            return tr("Timestamp");
        case ColumnIndex::AutoSyncStatus:
            return tr("Auto-Sync");
        case ColumnIndex::Progress:
            return tr("Progress");
        case ColumnIndex::CurrentVersion:
            return tr("Current Version");
        case ColumnIndex::Action:
            return tr("Action");
        case ColumnIndex::NoteNumber:
            return tr("Note");
        default:
            break;
        }
    }
    return QVariant();
}

QVariant TableModelFileMonitor::data(const QModelIndex &index, int role) const
{
    if(role == Qt::ItemDataRole::TextAlignmentRole)
    {
        if(index.column() == ColumnIndex::Type ||
           index.column() == ColumnIndex::Status ||
           index.column() == ColumnIndex::Timestamp ||
            index.column() == ColumnIndex::AutoSyncStatus)
        {
            return Qt::AlignmentFlag::AlignCenter;
        }
    }

    if(role == Qt::ItemDataRole::DecorationRole)
    {
        QFileIconProvider provider;

        if(index.column() == ColumnIndex::Name)
        {
            QModelIndex typeIndex = index.siblingAtColumn(ColumnIndex::Type);
            QVariant typeValue = QSqlQueryModel::data(typeIndex);
            ItemType type = typeValue.value<ItemType>();

            if(type == ItemType::File)
            {
                QModelIndex parentDirIndex = index.siblingAtColumn(ColumnIndex::ParentDir);
                auto parentDirValue = QSqlQueryModel::data(parentDirIndex).toString();
                auto nameValue = QSqlQueryModel::data(index).toString();

                QFileInfo info(parentDirValue + nameValue);
                auto result = provider.icon(info);
                return result;
            }
            else if(type == ItemType::Folder)
            {
                auto result = provider.icon(QFileIconProvider::IconType::Folder);
                return result;
            }
        }
        else if(index.column() == ColumnIndex::ParentDir)
        {
            auto result = provider.icon(QFileIconProvider::IconType::Folder).pixmap(32, 32);
            return result;
        }
    }

    QVariant value = QSqlQueryModel::data(index, role);

    if (value.isValid() && role == Qt::DisplayRole)
    {
        if (index.column() == ColumnIndex::Name)
            return value.toString();

        else if (index.column() == ColumnIndex::ParentDir)
            return value.toString();

        else if(index.column() == ColumnIndex::Path)
            return value.toString();

        else if(index.column() == ColumnIndex::OldName)
            return value.toString();

        else if(index.column() == ColumnIndex::Type)
        {
            if(value.value<ItemType>() == ItemType::File)
                return tr("File");
            else if(value.value<ItemType>() == ItemType::Folder)
                return tr("Folder");
            else
                return "NaN";
        }
        else if(index.column() == ColumnIndex::Status)
        {
            if(value.value<ItemStatus>() == ItemStatus::Modified)
                return STATUS_TEXT_MODIFIED;

            else if(value.value<ItemStatus>() == ItemStatus::NewAdded)
                return STATUS_TEXT_NEW_ADDED;

            else if(value.value<ItemStatus>() == ItemStatus::Deleted)
                return STATUS_TEXT_DELETED;

            else if(value.value<ItemStatus>() == ItemStatus::Moved)
                return STATUS_TEXT_MOVED;

            else if(value.value<ItemStatus>() == ItemStatus::MovedAndModified)
                return STATUS_TEXT_MOVED_AND_MODIFIED;

            else if(value.value<ItemStatus>() == ItemStatus::Missing)
                return STATUS_TEXT_MISSING;

            else if(value.value<ItemStatus>() == ItemStatus::InvalidStatus)
                return STATUS_TEXT_INVALID;

            else
                return tr("NaN");
        }
        else if(index.column() == ColumnIndex::Timestamp)
            return value.toDateTime();
        else if(index.column() == ColumnIndex::AutoSyncStatus)
        {
            if(value.toBool())
                return AUTO_SYNC_STATUS_ENABLED_TEXT;
            else
                return AUTO_SYNC_STATUS_DISABLED_TEXT;
        }
    }

    return value;
}