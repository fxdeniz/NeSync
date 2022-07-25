#ifndef V2TABLEMODELFILEMONITOR_H
#define V2TABLEMODELFILEMONITOR_H

#include <QSqlQueryModel>

class V2TableModelFileMonitor : public QSqlQueryModel
{
    Q_OBJECT
public:
    enum TableItemStatus
    {
        InvalidStatus,
        Missing,
        NewAdded,
        Modified,
        Moved,
        MovedAndModified,
        Deleted
    };

    enum TableItemType
    {
        UndefinedType,
        Folder,
        File
    };

public:
    V2TableModelFileMonitor(QObject *parent = nullptr);

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex &index, int role) const override;

signals:

};

#endif // V2TABLEMODELFILEMONITOR_H
