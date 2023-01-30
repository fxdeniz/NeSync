#ifndef TREEITEM_H
#define TREEITEM_H

#include <QList>
#include <QVariant>

class TreeItem
{
public:
    static const inline int ColumnIndexUserPath = 0;
    static const inline int ColumnIndexStatus = 1;
    static const inline int ColumnIndexDescription = 2;
    static const inline int ColumnIndexAction = 3;

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
    QVariant data(int column) const;
    int row() const;

    int getStatus() const;
    void setStatus(int newStatus);

private:
    QString userPath;
    int status;
    QString oldName;
    QString description;
    QString action;
    QList<TreeItem *> childItems;
    TreeItem *parentItem;
};

#endif // TREEITEM_H
