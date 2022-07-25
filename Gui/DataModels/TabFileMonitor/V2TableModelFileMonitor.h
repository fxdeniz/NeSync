#ifndef V2TABLEMODELFILEMONITOR_H
#define V2TABLEMODELFILEMONITOR_H

#include <QSqlQueryModel>

class V2TableModelFileMonitor : public QSqlQueryModel
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
    static const QString COLUMN_NAME_ACTION;
    static const QString COLUMN_NAME_NOTE_NUMBER;

    enum ColumnIndex
    {
        Name,
        ParentDir,
        Path,
        OldName,
        Type,
        Status,
        Timestamp,
        Action,
        NoteNumber
    };

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
