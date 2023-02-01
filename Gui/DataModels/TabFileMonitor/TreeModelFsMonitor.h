#ifndef TREEMODELFOLDERMONITOR_H
#define TREEMODELFOLDERMONITOR_H

#include "TreeItem.h"

#include "Backend/FileMonitorSubSystem/FileSystemEventDb.h"

#include <QStringListModel>
#include <QAbstractItemModel>

class TreeModelFsMonitor : public QAbstractItemModel
{
    Q_OBJECT

public:
    static const inline int ColumnIndexUserPath = 0;
    static const inline int ColumnIndexStatus = 1;
    static const inline int ColumnIndexAction = 2;
    static const inline int ColumnIndexDescription = 3;

    explicit TreeModelFsMonitor(QObject *parent = nullptr);
    ~TreeModelFsMonitor();

    void appendDescription();
    void updateDescription(int number, const QString &data);
    void deleteDescription(int number);
    QString getDescription(int number) const;
    QStringListModel *getDescriptionNumberListModel() const;

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
    void setupModelData();
    TreeItem *createTreeItem(const QString &pathToFileOrFolder, TreeItem::ItemType type, TreeItem *root) const;
    QString itemStatusToString(FileSystemEventDb::ItemStatus status) const;

    TreeItem *treeRoot;
    FileSystemEventDb *fsEventDb;
    QMap<int, QString> descriptionMap;
    QStringListModel *descriptionNumberListModel;

};

#endif // TREEMODELFOLDERMONITOR_H
