#ifndef TREEMODELDIALOGIMPORT_H
#define TREEMODELDIALOGIMPORT_H

#include <QJsonArray>
#include <QJsonObject>
#include <QAbstractItemModel>

#include "TreeItem.h"

namespace TreeModelDialogImport
{
    class Model;
};

class TreeModelDialogImport::Model : public QAbstractItemModel
{
    Q_OBJECT

public:
    static const inline int ColumnIndexSymbolPath = 0;
    static const inline int ColumnIndexStatus = 1;
    static const inline int ColumnIndexAction = 2;
    static const inline int ColumnIndexResult = 3;

    explicit Model(QJsonArray array, QObject *parent = nullptr);
    ~Model();

    QMap<QString, TreeItem *> getFolderItemMap() const;
    void disableComboBoxes();
    void markFileAsPending(const QString &symbolFilePath);
    void markFileAsSuccessful(const QString &symbolFilePath);
    void markFileAsFailed(const QString &symbolFilePath);

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

signals:
    void signalDisableItemDelegates();

private:
    TreeItem *createTreeItemFolder(const QString &symbolFolderPath, TreeItem *parentItem) const;
    TreeItem *createTreeItemFile(QJsonObject fileJson, TreeItem *parentItem) const;
    void markFile(const QString &symbolFilePath, TreeItem::Result result);

    TreeItem *treeRoot;
    QMap<QString, TreeItem *> folderItemMap;
    QHash<QString, TreeItem *> symbolFileMap; // Used for result update
};

#endif // TREEMODELDIALOGIMPORT_H
