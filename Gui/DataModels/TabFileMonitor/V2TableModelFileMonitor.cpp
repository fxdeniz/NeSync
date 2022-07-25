#include "V2TableModelFileMonitor.h"

#include <QColor>
#include <QDateTime>

V2TableModelFileMonitor::V2TableModelFileMonitor(QObject *parent)
    : QSqlQueryModel(parent)
{

}

QVariant V2TableModelFileMonitor::headerData(int section, Qt::Orientation orientation, int role) const
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

QVariant V2TableModelFileMonitor::data(const QModelIndex &index, int role) const
{
    QVariant value = QSqlQueryModel::data(index, role);

    if (value.isValid() && role == Qt::DisplayRole)
    {
        if (index.column() == 0)
            return value.toString();

        else if (index.column() == 1)
            return value.toString();

        else if(index.column() == 2)
            return value.toString();

        else if(index.column() == 3)
        {
            if(value.value<TableItemType>() == TableItemType::File)
                return tr("File");
            else if(value.value<TableItemType>() == TableItemType::Folder)
                return tr("Folder");
            else
                return "NaN";
        }
        else if(index.column() == 4)
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
        else if(index.column() == 5)
            return value.toDateTime();
    }

    return value;
}
