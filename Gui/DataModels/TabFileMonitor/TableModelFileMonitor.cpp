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
const QString TableModelFileMonitor::COLUMN_NAME_ACTION = "action";
const QString TableModelFileMonitor::COLUMN_NAME_NOTE_NUMBER = "note_number";

TableModelFileMonitor::TableModelFileMonitor(QObject *parent)
    : QSqlQueryModel(parent)
{
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
    if(role == Qt::ItemDataRole::DecorationRole)
    {
        QFileIconProvider provider;

        if(index.column() == ColumnIndex::Name)
        {
            QModelIndex typeIndex = index.siblingAtColumn(ColumnIndex::Type);
            QVariant typeValue = QSqlQueryModel::data(typeIndex);
            TableItemType type = typeValue.value<TableItemType>();

            if(type == TableItemType::File)
            {
                QModelIndex parentDirIndex = index.siblingAtColumn(ColumnIndex::ParentDir);
                auto parentDirValue = QSqlQueryModel::data(parentDirIndex).toString();
                auto nameValue = QSqlQueryModel::data(index).toString();

                QFileInfo info(parentDirValue + nameValue);
                auto result = provider.icon(info);
                return result;
            }
            else if(type == TableItemType::Folder)
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
            if(value.value<TableItemType>() == TableItemType::File)
                return tr("File");
            else if(value.value<TableItemType>() == TableItemType::Folder)
                return tr("Folder");
            else
                return "NaN";
        }
        else if(index.column() == ColumnIndex::Status)
        {
            if(value.value<TableItemStatus>() == TableItemStatus::Modified)
                return tr("Updated");

            else if(value.value<TableItemStatus>() == TableItemStatus::NewAdded)
                return tr("New Added");

            else if(value.value<TableItemStatus>() == TableItemStatus::Deleted)
                return tr("Deleted");

            else if(value.value<TableItemStatus>() == TableItemStatus::Moved)
                return tr("Moved");

            else if(value.value<TableItemStatus>() == TableItemStatus::MovedAndModified)
                return tr("Moved & Updated");

            else if(value.value<TableItemStatus>() == TableItemStatus::Missing)
                return tr("Missing");

            else if(value.value<TableItemStatus>() == TableItemStatus::InvalidStatus)
                return tr("Invalid");

            else
                return tr("NaN");
        }
        else if(index.column() == ColumnIndex::Timestamp)
            return value.toDateTime();
    }

    return value;
}
