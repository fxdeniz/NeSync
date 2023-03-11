#ifndef TREEITEM_H
#define TREEITEM_H

#include <Backend/FileMonitorSubSystem/FileSystemEventDb.h>

#include <QList>
#include <QVariant>

namespace TreeModelFileMonitor
{
    class TreeItem;
}

class TreeModelFileMonitor::TreeItem
{
public:

    enum ItemType
    {
        Undefined = 0,
        Folder = 1,
        File = 2
    };

    enum Action
    {
        NotSelected = 0,
        Save = 1,
        Restore = 2,
        Freeze = 3,
        Delete = 4
    };

    explicit TreeItem(TreeItem *parentItem = nullptr);
    ~TreeItem();

    TreeItem *getParentItem() const;
    void setParentItem(TreeItem *newParentItem);

    void setUserPath(const QString &newUserPath);
    QString getUserPath() const;
    void setDescription(const QString &newDescription);

    void appendChild(TreeItem *child);

    TreeItem *child(int row);
    int childCount() const;
    int columnCount() const;
    int row() const;

    FileSystemEventDb::ItemStatus getStatus() const;
    void setStatus(FileSystemEventDb::ItemStatus newStatus);

    ItemType getType() const;
    void setType(ItemType newType);

    Action getAction() const;
    void setAction(Action newAction);

    QString getDescription() const;

private:
    QString userPath;
    FileSystemEventDb::ItemStatus status;
    QString description;
    Action action;
    ItemType type;
    QList<TreeItem *> childItems;
    TreeItem *parentItem;
};

#endif // TREEITEM_H
