#ifndef TABLEMODELFILEEXPLORER_H
#define TABLEMODELFILEEXPLORER_H

#include <QAbstractTableModel>

class TableModelFileExplorer : public QAbstractTableModel
{
    Q_OBJECT

public:
    struct TableItem
    {
        QString name;
        QString itemCount;

        bool operator==(const TableItem &other) const
        {
            return name == other.name && itemCount == other.itemCount;
        }
    };

public:
    TableModelFileExplorer(QObject *parent = nullptr);
    TableModelFileExplorer(const QList<TableItem> &_itemList, QObject *parent = nullptr);

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
    QList<TableItem> itemList;
    QSet<QPersistentModelIndex> checkedItems;

};

#endif // TABLEMODELFILEEXPLORER_H
