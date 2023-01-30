#include "TreeModelFsMonitor.h"

#include "Utility/DatabaseRegistry.h"

TreeModelFsMonitor::TreeModelFsMonitor(QObject *parent)
    : QAbstractItemModel(parent)
{
    treeRoot = new TreeItem();
    fsEventDb = new FileSystemEventDb(DatabaseRegistry::fileSystemEventDatabase());

    setupModelData();
}

TreeModelFsMonitor::~TreeModelFsMonitor()
{
    delete treeRoot;
    delete fsEventDb;
}

int TreeModelFsMonitor::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    return treeRoot->columnCount();
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
        parentItem = treeRoot;
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

    if (parentItem == treeRoot)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int TreeModelFsMonitor::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = treeRoot;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

void TreeModelFsMonitor::setupModelData()
{
    QStringList rootFolders = fsEventDb->getActiveRootFolderList();

    for(const QString &currentRootFolderPath : rootFolders)
    {
        TreeItem *activeRoot = createTreeItemForFolder(currentRootFolderPath, treeRoot);
        treeRoot->appendChild(activeRoot);

        QStringList childFileList = fsEventDb->getChildFileListOfFolder(currentRootFolderPath);

        for(const QString &currentChildPath : childFileList)
        {
            TreeItem *childTreeItem = new TreeItem(activeRoot);
            childTreeItem->setUserPath(currentChildPath);

            auto status = fsEventDb->getStatusOfFile(currentChildPath);
            childTreeItem->setStatus(itemStatusToString(status));

            childTreeItem->setOldName(fsEventDb->getOldNameOfFile(currentChildPath));

            activeRoot->appendChild(childTreeItem);
        }
    }
}

TreeItem *TreeModelFsMonitor::createTreeItemForFolder(const QString &pathToFolder, TreeItem *root) const
{
    TreeItem *result = new TreeItem(root);
    result->setUserPath(pathToFolder);

    auto status = fsEventDb->getStatusOfFolder(pathToFolder);
    result->setStatus(itemStatusToString(status));

    QString oldName = fsEventDb->getOldNameOfFolder(pathToFolder);
    if(!oldName.isEmpty())
        result->setOldName(oldName);

    return result;
}

QString TreeModelFsMonitor::itemStatusToString(FileSystemEventDb::ItemStatus status) const
{
    QString result;

    if(status == FileSystemEventDb::ItemStatus::NewAdded)
        result = "New Added";
    else if(status ==  FileSystemEventDb::ItemStatus::Updated)
        result = "Updated";
    else if(status == FileSystemEventDb::ItemStatus::Renamed)
        result = "Renamed";
    else if(status == FileSystemEventDb::ItemStatus::UpdatedAndRenamed)
        result = "Updated & Renamed";
    else if(status == FileSystemEventDb::ItemStatus::Deleted)
        result = "Deleted";
    else if(status == FileSystemEventDb::ItemStatus::Undefined)
        result = "";
    else
        result = "NaN";

    return result;
}
