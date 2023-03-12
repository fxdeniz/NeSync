#include "TreeModelDialogImport.h"
#include "Utility/JsonDtoFormat.h"
#include "Backend/FileStorageSubSystem/FileStorageManager.h"

#include <QDir>
#include <QFileIconProvider>

using namespace TreeModelDialogImport;

Model::Model(QJsonArray array, QObject *parent) : QAbstractItemModel(parent)
{
    treeRoot = new TreeItem();

    QMultiMap<QString, TreeItem *> multiFolderMap;

    for(const QJsonValue &currentValue : array)
    {
        QJsonObject fileJson = currentValue.toObject();
        TreeItem *item = createTreeItemFile(fileJson, nullptr);

        multiFolderMap.insert(fileJson[JsonKeys::File::SymbolFolderPath].toString(), item);
    }

    for(const QString &symbolFolderPath : multiFolderMap.uniqueKeys())
    {
        TreeItem *currentFolder = createTreeItemFolder(symbolFolderPath, treeRoot);
        treeRoot->appendChild(currentFolder);

        for(TreeItem *item : multiFolderMap.values(symbolFolderPath))
        {
            item->setParentItem(currentFolder);
            currentFolder->appendChild(item);
        }

        folderItemMap.insert(symbolFolderPath,currentFolder);
    }
}

Model::~Model()
{
    delete treeRoot;
}

QMap<QString, TreeItem *> Model::getFolderItemMap() const
{
    return folderItemMap;
}

void Model::disableComboBoxes()
{
    emit signalDisableItemDelegates();
}

QVariant Model::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
    {
        switch (section)
        {
            case ColumnIndexSymbolPath:
                return tr("Symbol Path");
            case ColumnIndexStatus:
                return tr("Status");
            case ColumnIndexAction:
                return tr("Action");
            default:
                break;
        }
    }

    return QVariant();}

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

    if(role == Qt::ItemDataRole::DecorationRole && index.column() == ColumnIndexSymbolPath)
    {
        QFileIconProvider provider;

        if(item->getType() == TreeItem::ItemType::Folder)
            return provider.icon(QFileIconProvider::IconType::Folder);
        else
        {
            QFileInfo info(item->getName());
            return provider.icon(info);
        }
    }
    else if(role == Qt::ItemDataRole::DisplayRole)
    {
        if(index.column() == ColumnIndexSymbolPath)
        {

            if(item->getType() == TreeItem::ItemType::File)
                return item->getName();

            else if(item->getType() == TreeItem::ItemType::Folder)
                return item->getSymbolFolderPath();
        }
        else if(index.column() == ColumnIndexStatus)
        {
            if(item->getType() == TreeItem::ItemType::File)
            {
                if(item->getStatus() == TreeItem::Status::NewFile)
                    return tr("New File");
                else if(item->getStatus() == TreeItem::Status::ExistingFile)
                    return tr("Existing file");
            }
            else if(item->getType() == TreeItem::ItemType::Folder)
            {
                if(item->getStatus() == TreeItem::Status::NewFolder)
                    return tr("New Folder");
                else if(item->getStatus() == TreeItem::Status::ExistingFolder)
                    return tr("Existing Folder");
            }
        }
    }
    else if (role == Qt::ItemDataRole::BackgroundRole)
    {
        if(item->getStatus() == TreeItem::Status::NewFile)
            return QColor::fromString("#b8e994");
        else if(item->getStatus() == TreeItem::Status::NewFolder)
            return QColor::fromString("#78e08f");
        else if(item->getStatus() == TreeItem::Status::ExistingFolder)
            return QColor::fromString("#82ccdd");
    }
    else if(role == Qt::ItemDataRole::TextAlignmentRole && index.column() == ColumnIndexStatus)
        return Qt::AlignmentFlag::AlignCenter;

    return QVariant();
}

TreeItem *Model::createTreeItemFolder(const QString &symbolFolderPath, TreeItem *parentItem) const
{
    TreeItem *result = new TreeItem(parentItem);
    result->setSymbolFolderPath(symbolFolderPath);
    result->setType(TreeItem::ItemType::Folder);

    auto fsm = FileStorageManager::instance();

    if(fsm->getFolderJsonBySymbolPath(symbolFolderPath)[JsonKeys::IsExist].toBool())
        result->setStatus(TreeItem::Status::ExistingFolder);
    else
        result->setStatus(TreeItem::Status::NewFolder);

    return result;
}

TreeItem *Model::createTreeItemFile(QJsonObject fileJson, TreeItem *parentItem) const
{
    TreeItem *result = new TreeItem(parentItem);
    result->setName(fileJson[JsonKeys::File::FileName].toString());
    result->setSymbolFolderPath(fileJson[JsonKeys::File::SymbolFolderPath].toString());
    result->setType(TreeItem::ItemType::File);
    result->setFileJson(fileJson);

    QString symbolFilePath = fileJson[JsonKeys::File::SymbolFilePath].toString();

    auto fsm = FileStorageManager::instance();

    if(fsm->getFileJsonBySymbolPath(symbolFilePath)[JsonKeys::IsExist].toBool())
        result->setStatus(TreeItem::Status::ExistingFile);
    else
        result->setStatus(TreeItem::Status::NewFile);

    return result;
}
