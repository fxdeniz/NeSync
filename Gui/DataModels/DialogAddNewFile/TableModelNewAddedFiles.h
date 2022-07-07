#ifndef TABLEMODELNEWADDEDFILES_H
#define TABLEMODELNEWADDEDFILES_H

#include <QAbstractTableModel>

class TableModelNewAddedFiles : public QAbstractTableModel
{
    Q_OBJECT

public:
    struct TableItem
    {
        QString fileName;
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
    QStringList getFilePathList() const;

private:
    QList<TableItem> itemList;
};

#endif // TABLEMODELNEWADDEDFILES_H
