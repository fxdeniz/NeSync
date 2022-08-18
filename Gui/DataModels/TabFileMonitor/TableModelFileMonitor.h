#ifndef TABLEMODELFILEMONITOR_H
#define TABLEMODELFILEMONITOR_H

#include <QSqlQueryModel>

class TableModelFileMonitor : public QSqlQueryModel
{
    Q_OBJECT
public:
    static const QString TABLE_NAME;
    static const QString COLUMN_NAME_NAME;
    static const QString COLUMN_NAME_PARENT_DIR;
    static const QString COLUMN_NAME_PATH;
    static const QString COLUMN_NAME_OLD_NAME;
    static const QString COLUMN_NAME_TYPE;
    static const QString COLUMN_NAME_STATUS;
    static const QString COLUMN_NAME_TIMESTAMP;
    static const QString COLUMN_NAME_AUTOSYNC_STATUS;
    static const QString COLUMN_NAME_PROGRESS;
    static const QString COLUMN_NAME_CURRENT_VERSION;
    static const QString COLUMN_NAME_ACTION;
    static const QString COLUMN_NAME_NOTE_NUMBER;

    static const QString STATUS_TEXT_MODIFIED;
    static const QString STATUS_TEXT_NEW_ADDED;
    static const QString STATUS_TEXT_DELETED;
    static const QString STATUS_TEXT_MOVED;
    static const QString STATUS_TEXT_MOVED_AND_MODIFIED;
    static const QString STATUS_TEXT_MISSING;
    static const QString STATUS_TEXT_INVALID;

    static const QString AUTO_SYNC_STATUS_ENABLED_TEXT;
    static const QString AUTO_SYNC_STATUS_DISABLED_TEXT;

    enum ColumnIndex
    {
        Name = 0,
        ParentDir = 1,
        Path = 2,
        OldName = 3,
        Type = 4,
        Status = 5,
        Timestamp = 6,
        AutoSyncStatus = 7,
        Progress = 8,
        CurrentVersion = 9,
        Action = 10,
        NoteNumber = 11,
    };

    enum Action
    {
        Update,
        Replace,
        Delete,
        Freeze
    };

    enum ItemStatus
    {
        InvalidStatus,
        Missing,
        NewAdded,
        Modified,
        Moved,
        MovedAndModified,
        Deleted
    };

    enum ItemType
    {
        UndefinedType,
        Folder,
        File
    };

public:
    TableModelFileMonitor(QObject *parent = nullptr);

    static ItemStatus statusCodeFromString(const QString &status);

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex &index, int role) const override;

signals:

};

#endif // TABLEMODELFILEMONITOR_H