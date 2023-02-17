#ifndef TABLEMODELFILEEXPLORER_H
#define TABLEMODELFILEEXPLORER_H

#include <QAbstractTableModel>
#include <QIcon>

class TableModelFileExplorer : public QAbstractTableModel
{
    Q_OBJECT

public:
    static const inline int ColumnIndexName = 0;
    static const inline int ColumnIndexSymbolPath = 1;
    static const inline int ColumnIndexUserPath = 2;
    static const inline int ColumnIndexItemType = 3;

    enum TableItemType
    {
        Invalid,
        Folder,
        File
    };

    struct TableItem
    {
        QString name;
        QString symbolPath;
        QString userPath;
        TableItemType type;
        QIcon icon;

        bool operator==(const TableItem &other) const
        {
            return name == other.name && symbolPath == other.symbolPath;
        }
    };

public:
    TableModelFileExplorer(QJsonObject result, QObject *parent = nullptr);
    QString symbolPathFromModelIndex(const QModelIndex &index) const;
    TableItemType itemTypeFromModelIndex(const QModelIndex &index) const;

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
    static QList<TableItem> tableItemListFrom(QJsonObject parentFolder);

private:
    QList<TableItem> itemList;
    QSet<QPersistentModelIndex> checkedItems;

};

#endif // TABLEMODELFILEEXPLORER_H
