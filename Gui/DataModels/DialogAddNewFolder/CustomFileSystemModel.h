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
        Frozen = 4,
        Status = 5
    };

    enum ItemStatus
    {
        NotApplicable,
        Waiting,
        Pending,
        Successful,
        Failed
    };

    explicit CustomFileSystemModel(QObject *parent = nullptr);

    // QFileSystemModel interface
public:
    void updateFrozenStatusOfItem(const QModelIndex &index);

    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    bool isFileMarkedAsFrozen(const QString &pathToFile);

public slots:
    void markItemAsPending(const QString &pathToFile);
    void markItemAsSuccessful(const QString &pathToFile);
    void markItemAsFailed(const QString &pathToFile);

private slots:
    void addToStatusColumn(const QModelIndex &index, int first, int last);

private:
    static QString itemStatusToString(ItemStatus status);
    void markItem(const QString &pathToFile, ItemStatus status);
    QSet<QString> frozenFiles;
    QHash<QString, ItemStatus> statusOfFiles;
};

#endif // CUSTOMFILESYSTEMMODEL_H
