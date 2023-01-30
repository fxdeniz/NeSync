#include "TreeModelFsMonitor.h"

#include "Utility/DatabaseRegistry.h"

#include <QDir>
#include <QStack>
#include <QFileIconProvider>

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

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    if(role == Qt::ItemDataRole::DecorationRole && index.column() == 0)
    {
        QFileIconProvider provider;
        QFileInfo info(item->getUserPath());
        return provider.icon(info);
    }
    else if(role == Qt::ItemDataRole::DisplayRole)
    {
        if(index.column() == 0)
        {
            if(item->getParentItem() == treeRoot)
                return item->getUserPath();
            else
            {
                QString userPath = item->getUserPath();
                userPath.chop(1); // Remove QDir::seperator()
                return userPath.split(QDir::separator()).last();
            }
        }
    }

    return QVariant();
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
    TreeItem *parentItem = childItem->getParentItem();

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
        TreeItem *activeRoot = createTreeItem(currentRootFolderPath, true, nullptr);
        QStack<TreeItem *> itemStack;
        itemStack.push(activeRoot);

        while(!itemStack.isEmpty())
        {
            TreeItem *parentItem = itemStack.pop();

            if(parentItem->getParentItem() == nullptr)
            {
                parentItem->setParentItem(treeRoot);
                treeRoot->appendChild(activeRoot);
            }

            QStringList childFolderPathList = fsEventDb->getDirectChildFolderListOfFolder(parentItem->getUserPath());
            QStringList eventFulFileList = fsEventDb->getEventfulFileListOfFolder(parentItem->getUserPath());

            for(const QString &childFilePath : eventFulFileList)
            {
                TreeItem *fileItem = createTreeItem(childFilePath, false, parentItem);
                parentItem->appendChild(fileItem);
            }

            for(const QString &currentChildFolderPath : childFolderPathList)
            {
                TreeItem *childItem = createTreeItem(currentChildFolderPath, true, parentItem);
                parentItem->appendChild(childItem);
                itemStack.push(childItem);
            }
        }
    }
}

TreeItem *TreeModelFsMonitor::createTreeItem(const QString &pathToFileOrFolder, bool shouldCreateFolder, TreeItem *root) const
{
    TreeItem *result = new TreeItem(root);
    result->setUserPath(pathToFileOrFolder);

    auto status = fsEventDb->getStatusOfFolder(pathToFileOrFolder);
    result->setStatus(status);

    QString oldName;

    if(shouldCreateFolder)
        oldName = fsEventDb->getOldNameOfFolder(pathToFileOrFolder);
    else
        oldName = fsEventDb->getOldNameOfFile(pathToFileOrFolder);

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
