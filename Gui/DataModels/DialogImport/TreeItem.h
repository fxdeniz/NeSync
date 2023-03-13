#ifndef DIALOG_IMPORT_TREEITEM_H
#define DIALOG_IMPORT_TREEITEM_H

#include <QList>
#include <QVariant>
#include <QModelIndex>
#include <QJsonObject>

namespace TreeModelDialogImport
{
    class TreeItem;
}

class TreeModelDialogImport::TreeItem
{
public:
    static const inline int ColumnCount = 4;

    enum ItemType
    {
        Undefined = 0,
        Folder = 1,
        File = 2
    };

    enum Status
    {
        NotSet = 0,
        NewFolder = 1,
        ExistingFolder = 2,
        NewFile = 3,
        ExistingFile = 4
    };

    enum Action
    {
        NotSelected = 0,
        Import = 1,
        Overwrite = 2,
        ChooseEachChildren = 3,
        DoNotImport = 4
    };

    enum Result
    {
        Waiting = 0,
        Pending = 1,
        Successful = 2,
        Failed = 3
    };

    explicit TreeItem(TreeItem *parentItem = nullptr);
    ~TreeItem();

    TreeItem *getParentItem() const;
    void setParentItem(TreeItem *newParentItem);

    QString getSymbolFolderPath() const;
    void setSymbolFolderPath(const QString &newSymbolFolderPath);

    QString getName() const;
    void setName(const QString &newName);

    ItemType getType() const;
    void setType(ItemType newType);

    Action getAction() const;
    void setAction(Action newAction);

    Status getStatus() const;
    void setStatus(Status newStatus);

    Result getResult() const;
    void setResult(Result newResult);

    QJsonObject getFileJson() const;
    void setFileJson(const QJsonObject &newFileJson);

    QModelIndex getModelIndex() const;
    void setModelIndex(const QModelIndex &newModelIndex);

    void appendChild(TreeItem *child);

    TreeItem *child(int row);
    int childCount() const;
    int columnCount() const;
    int row() const;

private:
    QString symbolFolderPath;
    QString name;
    ItemType type;
    Action action;
    Status status;
    Result result;
    QJsonObject fileJson;
    QModelIndex modelIndex;
    QList<TreeItem *> childItems;
    TreeItem *parentItem;
};

#endif // DIALOG_IMPORT_TREEITEM_H
