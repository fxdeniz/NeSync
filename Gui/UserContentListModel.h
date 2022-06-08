#ifndef USERCONTENTLISTMODEL_H
#define USERCONTENTLISTMODEL_H

#include <QAbstractListModel>

// code here is taken from
// http://www.java2s.com/Code/Cpp/Qt/stringlistmodelexample.htm
// https://www.walletfox.com/course/qtcheckablelist.php
class UserContentListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    UserContentListModel(QObject *parent = nullptr);
    UserContentListModel(const QStringList &itemList, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex()) override;
    bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex()) override;

private:
    QStringList stringList;
    QSet<QPersistentModelIndex> checkedItems;
};

#endif // USERCONTENTLISTMODEL_H
