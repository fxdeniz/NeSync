#ifndef TREEITEM_H
#define TREEITEM_H

#include <QList>
#include <QVariant>

class TreeItem
{
public:

    enum ItemType
    {
        Undefined = 0,
        Folder = 1,
        File = 2
    };

    explicit TreeItem(TreeItem *parentItem = nullptr);
    ~TreeItem();

    TreeItem *getParentItem() const;
    void setParentItem(TreeItem *newParentItem);

    void setUserPath(const QString &newUserPath);
    QString getUserPath() const;
    void setOldName(const QString &newOldName);
    void setDescription(const QString &newDescription);
    void setAction(const QString &newAction);

    void appendChild(TreeItem *child);

    TreeItem *child(int row);
    int childCount() const;
    int columnCount() const;
    int row() const;

    int getStatus() const;
    void setStatus(int newStatus);

    ItemType getType() const;
    void setType(ItemType newType);

private:
    QString userPath;
    int status;
    QString oldName;
    QString description;
    QString action;
    ItemType type;
    QList<TreeItem *> childItems;
    TreeItem *parentItem;
};

#endif // TREEITEM_H
