#ifndef TABLEMODELFILEMONITOR_H
#define TABLEMODELFILEMONITOR_H

#include <QDateTime>
#include <QAbstractTableModel>

class TableModelFileMonitor : public QAbstractTableModel
{
    Q_OBJECT

public:
    struct MonitorTableItem
    {
        QString fileName;
        QString folderPath;
        QString eventType;
        QDateTime timestamp;

        bool operator==(const MonitorTableItem &other) const
        {
            return fileName == other.fileName && folderPath == other.folderPath;
        }
    };

public:
    TableModelFileMonitor(QObject *parent = nullptr);
    TableModelFileMonitor(const QList<MonitorTableItem> &_itemList, QObject *parent = nullptr);

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
    QList<MonitorTableItem> itemList;
    QSet<QPersistentModelIndex> checkedItems;

};

#endif // TABLEMODELFILEMONITOR_H
