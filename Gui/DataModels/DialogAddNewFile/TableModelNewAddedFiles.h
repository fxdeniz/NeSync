#ifndef TABLEMODELNEWADDEDFILES_H
#define TABLEMODELNEWADDEDFILES_H

#include <QAbstractTableModel>

class TableModelNewAddedFiles : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum TableItemStatus
    {
        Waiting,
        Pending,
        Successful,
        Failed,
    };

    struct TableItem
    {
        QString fileName;
        bool isAutoSyncEnabled;
        TableItemStatus status;
        QString location;

        bool operator==(const TableItem &other) const
        {
            return fileName == other.fileName && location == other.location;
        }
    };

public:
    TableModelNewAddedFiles(QObject *parent = nullptr);
    TableModelNewAddedFiles(const QList<TableItem> &contacts, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex()) override;
    bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex()) override;

    const QList<TableItem> &getItemList() const;

public slots:
    void markItemAsPending(const QString &pathToFile);
    void markItemAsSuccessful(const QString &pathToFile);
    void markItemAsFailed(const QString &pathToFile);

private:
    void markItemAs(const QString &pathToFile, TableItemStatus status);

private:
    QList<TableItem> itemList;

};

#endif // TABLEMODELNEWADDEDFILES_H
