#include "TreeModelFsMonitor.h"

#include "Backend/FileMonitorSubSystem/FileSystemEventDb.h"

#include <QSqlQuery>
#include <QSqlRecord>

TreeModelFsMonitor::TreeModelFsMonitor(const QSqlDatabase &db, QObject *parent)
    : QAbstractItemModel(parent)
{
    rootItem = new TreeItem();
    database = db;

    if(!database.isOpen())
        database.open();

    setupModelData();
}

TreeModelFsMonitor::~TreeModelFsMonitor()
{
    delete rootItem;
}

int TreeModelFsMonitor::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    return rootItem->columnCount();
}

QVariant TreeModelFsMonitor::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    return item->data(index.column());
}

Qt::ItemFlags TreeModelFsMonitor::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index);
}

QVariant TreeModelFsMonitor::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
    {
        switch (section)
        {
            case TreeItem::ColumnIndexUserPath:
                return tr("Path");
            case TreeItem::ColumnIndexStatus:
                return tr("Status");
            case TreeItem::ColumnIndexDescription:
                return tr("Description");
            case TreeItem::ColumnIndexAction:
                return tr("Action");
            default:
                break;
        }
    }

    return QVariant();
}

QModelIndex TreeModelFsMonitor::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    return QModelIndex();
}

QModelIndex TreeModelFsMonitor::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parentItem();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int TreeModelFsMonitor::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

void TreeModelFsMonitor::setupModelData()
{
    QString queryTemplate = "SELECT * FROM Folder WHERE efsw_id IS NOT NULL;" ;
    QSqlQuery query(database);
    query.prepare(queryTemplate);
    query.exec();

    while(query.next())
    {
        QSqlRecord record = query.record();

        TreeItem *item = new TreeItem(rootItem);
        item->setUserPath(record.value("folder_path").toString());

        auto status = record.value("status").value<FileSystemEventDb::ItemStatus>();

        if(status == FileSystemEventDb::ItemStatus::NewAdded)
            item->setStatus("New Added");
        else if(status ==  FileSystemEventDb::ItemStatus::Updated)
            item->setStatus("Updated");
        else if(status == FileSystemEventDb::ItemStatus::Renamed)
            item->setStatus("Renamed");
        else if(status == FileSystemEventDb::ItemStatus::UpdatedAndRenamed)
            item->setStatus("Updated & Renamed");
        else if(status == FileSystemEventDb::ItemStatus::Deleted)
            item->setStatus("Deleted");
        else if(status == FileSystemEventDb::ItemStatus::Undefined)
            item->setStatus("");
        else
            item->setStatus("NaN");

        QString oldName = record.value("old_name").toString();
        if(!oldName.isEmpty())
            item->setOldName(oldName);

        rootItem->appendChild(item);
    }
}
