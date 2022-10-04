#ifndef CUSTOMFILESYSTEMMODEL_H
#define CUSTOMFILESYSTEMMODEL_H

#include <QFileSystemModel>

class CustomFileSystemModel : public QFileSystemModel
{
    Q_OBJECT
public:

    enum ColumnIndex
    {
        Name = 0,
        Size = 1,
        Type = 2,
        DateModified = 3,
        AutoSync = 4,
        Status = 5
    };

    enum ItemStatus
    {
        Waiting,
        Pending,
        Successful,
        Failed,
    };

    explicit CustomFileSystemModel(QObject *parent = nullptr);

    // QFileSystemModel interface
public:
    void updateAutoSyncStatusOfItem(const QModelIndex &index);

    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
    static QString itemStatusToString(ItemStatus status);
    QSet<QString> autoSyncDisabledFiles;
    QHash<QString, ItemStatus> statusOfFiles;
};

#endif // CUSTOMFILESYSTEMMODEL_H
