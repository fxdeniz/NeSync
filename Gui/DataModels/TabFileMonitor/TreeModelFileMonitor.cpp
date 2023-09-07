#include "TreeModelFileMonitor.h"

#include "Utility/DatabaseRegistry.h"

#include <QDir>
#include <QStack>
#include <QFileIconProvider>

using namespace TreeModelFileMonitor;

Model::Model(QObject *parent) : QAbstractItemModel(parent)
{
    treeRoot = new TreeItem();
    fsEventDb = new FileSystemEventDb(DatabaseRegistry::fileSystemEventDatabase());
    descriptionNumberListModel = new QStringListModel(this);
    setupModelData();
}

Model::~Model()
{
    delete treeRoot;
    delete fsEventDb;
}

void Model::disableComboBoxes()
{
    emit signalDisableItemDelegates();
}

void Model::appendDescription()
{
    if(descriptionMap.isEmpty())
    {
        descriptionMap.insert(1, "");
        descriptionNumberListModel->setStringList(QStringList{ QString::number(1) });
    }
    else
    {
        int number = descriptionMap.lastKey() + 1;
        descriptionMap.insert(number, "");

        auto list = descriptionNumberListModel->stringList();
        list.append(QString::number(number));
        descriptionNumberListModel->setStringList(list);
    }
}

void Model::updateDescription(int number, const QString &data)
{
    bool isContains = descriptionMap.contains(number);

    if(isContains)
        descriptionMap.insert(number, data);
}

void Model::deleteDescription(int number)
{
    if(!descriptionMap.isEmpty())
    {
        descriptionMap.remove(number);
        auto list = descriptionNumberListModel->stringList();
        list.removeOne(QString::number(number));
        descriptionNumberListModel->setStringList(list);
    }
}

bool Model::isDescriptionExist(int number) const
{
    return descriptionMap.contains(number);
}

QString Model::getDescription(int number) const
{
    QString result;

    bool isContains = descriptionMap.contains(number);

    if(isContains)
        result = descriptionMap.value(number);

    return result;
}

int Model::getMaxDescriptionNumber() const
{
    int result = -1;

    if(!descriptionMap.isEmpty())
        result = descriptionMap.lastKey();

    return result;
}

QStringListModel *Model::getDescriptionNumberListModel() const
{
    return descriptionNumberListModel;
}

QMap<QString, TreeItem *> Model::getFileItemMap() const
{
    return fileItemMap;
}

int Model::getTotalItemCount() const
{
    return folderItemMap.size() + fileItemMap.size();
}

QMap<QString, TreeItem *> Model::getFolderItemMap() const
{
    return folderItemMap;
}

int Model::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    return treeRoot->columnCount();
}

QVariant Model::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    if(role == Qt::ItemDataRole::DecorationRole && index.column() == 0)
    {
        QFileIconProvider provider;

        if(item->getType() == TreeItem::ItemType::Folder)
            return provider.icon(QFileIconProvider::IconType::Folder);
        else
        {
            QFileInfo info(item->getUserPath());
            return provider.icon(info);
        }
    }
    else if(role == Qt::ItemDataRole::DisplayRole)
    {
        if(index.column() == ColumnIndexUserPath)
        {
            if(item->getType() == TreeItem::ItemType::Folder)
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
            else if(item->getType() == TreeItem::ItemType::File)
            {
                QString userPath = item->getUserPath();
                return userPath.split(QDir::separator()).last();
            }
        }
        else if(index.column() == ColumnIndexStatus)
        {
            QString result = itemStatusToString(item->getStatus());
            return result;
        }
    }

    return QVariant();
}

Qt::ItemFlags Model::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index);
}

QVariant Model::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
    {
        switch (section)
        {
            case ColumnIndexUserPath:
                return tr("Path");
            case ColumnIndexStatus:
                return tr("Status");
            case ColumnIndexDescription:
                return tr("Description");
            case ColumnIndexAction:
                return tr("Action");
            default:
                break;
        }
    }

    return QVariant();
}

QModelIndex Model::index(int row, int column, const QModelIndex &parent) const
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

QModelIndex Model::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->getParentItem();

    if (parentItem == treeRoot)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int Model::rowCount(const QModelIndex &parent) const
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

void Model::setupModelData()
{
    QStringList rootFolders = fsEventDb->getMonitoredRootFolderList();
    rootFolders.append(fsEventDb->getMissingRootFolderList());

    for(const QString &currentRootFolderPath : rootFolders)
    {
        TreeItem *activeRoot = createTreeItem(currentRootFolderPath, TreeItem::ItemType::Folder, nullptr);
        QStack<TreeItem *> itemStack;
        itemStack.push(activeRoot);

        while(!itemStack.isEmpty())
        {
            TreeItem *parentItem = itemStack.pop();

            if(parentItem->getParentItem() == nullptr)
            {
                parentItem->setParentItem(treeRoot);
                treeRoot->appendChild(activeRoot);
                folderItemMap.insert(parentItem->getUserPath(), parentItem);
            }

            QStringList childFolderPathList = fsEventDb->getDirectChildFolderListOfFolder(parentItem->getUserPath());
            QStringList eventFulFileList = fsEventDb->getEventfulFileListOfFolder(parentItem->getUserPath());

            for(const QString &childFilePath : eventFulFileList)
            {
                TreeItem *fileItem = createTreeItem(childFilePath, TreeItem::ItemType::File, parentItem);
                parentItem->appendChild(fileItem);
                fileItemMap.insert(fileItem->getUserPath(), fileItem);
            }

            for(const QString &currentChildFolderPath : childFolderPathList)
            {
                TreeItem *childItem = createTreeItem(currentChildFolderPath, TreeItem::ItemType::Folder, parentItem);
                parentItem->appendChild(childItem);
                itemStack.push(childItem);
                folderItemMap.insert(childItem->getUserPath(), childItem);
            }
        }
    }
}

TreeItem *Model::createTreeItem(const QString &pathToFileOrFolder, TreeItem::ItemType type, TreeItem *root) const
{
    TreeItem *result = new TreeItem(root);
    result->setUserPath(pathToFileOrFolder);

    auto status = FileSystemEventDb::ItemStatus::Invalid;

    if(type == TreeItem::ItemType::Folder)
        status = fsEventDb->getStatusOfFolder(pathToFileOrFolder);
    else
        status = fsEventDb->getStatusOfFile(pathToFileOrFolder);

    result->setStatus(status);
    result->setType(type);

    return result;
}

QString Model::itemStatusToString(FileSystemEventDb::ItemStatus status) const
{
    QString result;

    if(status == FileSystemEventDb::ItemStatus::NewAdded)
        result = "New Added";
    else if(status ==  FileSystemEventDb::ItemStatus::Updated)
        result = "Updated";
    else if(status == FileSystemEventDb::ItemStatus::Renamed)
        result = "Renamed";
    else if(status == FileSystemEventDb::ItemStatus::Deleted)
        result = "Deleted";
    else if(status == FileSystemEventDb::ItemStatus::Missing)
        result = "Missing";
    else if(status == FileSystemEventDb::ItemStatus::Monitored)
        result = "";
    else
        result = "NaN";

    return result;
}
