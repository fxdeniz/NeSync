#ifndef TREEMODELFOLDERMONITOR_H
#define TREEMODELFOLDERMONITOR_H

#include "TreeItem.h"

#include <QAbstractItemModel>

class TreeModelFsMonitor : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit TreeModelFsMonitor(const QString &data, QObject *parent = nullptr);
    ~TreeModelFsMonitor();

    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

private:
    void setupModelData(const QStringList &lines, TreeItem *parent);

    TreeItem *rootItem;

};

#endif // TREEMODELFOLDERMONITOR_H
