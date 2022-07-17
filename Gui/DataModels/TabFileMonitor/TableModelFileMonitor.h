#ifndef TABLEMODELFILEMONITOR_H
#define TABLEMODELFILEMONITOR_H

#include <QDateTime>
#include <QAbstractTableModel>

class TableModelFileMonitor : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum TableItemStatus
    {
        InvalidStatus,
        Missing,
        NewAdded,
        Updated,
        Moved,
        MovedAndUpdated,
        Deleted
    };

    enum TableItemType
    {
        UndefinedType,
        Folder,
        File
    };

    struct TableItem
    {
        QString name;
        QString parentDirPath;
        QString oldName;
        TableItemType itemType;
        TableItemStatus eventType;
        QDateTime timestamp;


        bool operator==(const TableItem &other) const
        {
            return name == other.name && parentDirPath == other.parentDirPath;
        }
    };

public:
    TableModelFileMonitor(QObject *parent = nullptr);
    TableModelFileMonitor(const QList<TableItem> &_itemList, QObject *parent = nullptr);

    static TableItem tableItemNewAddedFolderFrom(const QString &pathToFolder);
    static TableItem tableItemDeletedFolderFrom(const QString &pathToFolder);
    static TableItem tableItemMovedFolderFrom(const QString &pathToFolder, const QString &oldLocation);
    static TableItem tableItemNewAddedFileFrom(const QString &pathToFile);
    static TableItem tableItemDeletedFileFrom(const QString &pathToFile);
    static TableItem tableItemMovedFileFrom(const QString &pathToFile, const QString &oldLocation);
    static TableItem tableItemUpdatedFileFrom(const QString &pathToFile);
    static TableItem tableItemMovedAndUpdatedFileFrom(const QString &pathToFile, const QString &oldLocation);

    // QAbstractTableModel interface
public:
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex()) override;
    bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex()) override;

private:
    static TableItem tableItemFolderFrom(const QString &pathToFolder, TableItemStatus status, const QString &oldLocation = "");
    static TableItem tableItemFileFrom(const QString &pathToFile, TableItemStatus status, const QString &oldLocation = "");

private:
    QList<TableItem> itemList;
    QSet<QPersistentModelIndex> checkedItems;

};

#endif // TABLEMODELFILEMONITOR_H
