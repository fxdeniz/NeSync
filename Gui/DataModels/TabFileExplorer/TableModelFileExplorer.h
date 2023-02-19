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
    static const inline int ColumnIndexIsFrozen = 3;
    static const inline int ColumnIndexItemType = 4;

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
        bool isFrozen;
        TableItemType type;
        QIcon icon;

        bool operator==(const TableItem &other) const
        {
            return name == other.name && symbolPath == other.symbolPath;
        }
    };

public:
    TableModelFileExplorer(QJsonObject result, QObject *parent = nullptr);

    QString getNameFromModelIndex(const QModelIndex &index) const;
    QString getSymbolPathFromModelIndex(const QModelIndex &index) const;
    QString getUserPathFromModelIndex(const QModelIndex &index) const;
    bool getIsFrozenFromModelIndex(const QModelIndex &index) const;
    TableItemType getItemTypeFromModelIndex(const QModelIndex &index) const;

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
