#ifndef TABLEINFOFILEEVENT_H
#define TABLEINFOFILEEVENT_H

#include <QObject>
#include <QString>
#include <QSharedDataPointer>

inline const QString TABLE_NAME_FILE_EVENT = "FileEvent";
inline const QString TABLE_FILE_EVENT_COLNAME_EVENT_ID = "event_id";
inline const QString TABLE_FILE_EVENT_COLNAME_PARENT_RECORD_ID = "parent_record_id";
inline const QString TABLE_FILE_EVENT_COLNAME_PARENT_VERSION_ID = "parent_version_id";
inline const QString TABLE_FILE_EVENT_COLNAME_PARENT_EVENT_ID = "parent_event_id";
inline const QString TABLE_FILE_EVENT_COLNAME_TYPE = "type";
inline const QString TABLE_FILE_EVENT_COLNAME_STATUS = "status";
inline const QString TABLE_FILE_EVENT_COLNAME_TIMESTAMP = "timestamp";
inline const QString TABLE_FILE_EVENT_COLNAME_DATA = "data";

enum FileEventType
{
    InvalidEvent = 0,
    UnRegisteredFileEvent = 1,
    EventSequenceStartedEvent = 2,
    FileUpdatedEvent = 3,
    FileRenamedEvent = 4,
    FileMovedEvent = 5,
    FileDeletedEvent = 6
};

enum FileEventStatusCode
{
    FileRecordReserved = 21,
    WaitingForFileVersionCommit = 22,
    FileRecordCommitted = 23,

    FileVersionReserved = 31,
    FileCopied = 32,
    FileVersionCommitted = 33
};

Q_DECLARE_METATYPE(FileEventType)

inline const FileEventType NONEXIST_DEFAULT_EVENT_TYPE = FileEventType::InvalidEvent;

class RowFileEvent;

typedef QSharedPointer<RowFileEvent> PtrTo_RowFileEvent;
typedef QSharedPointer<const RowFileEvent> PtrToConst_RowFileEvent;
typedef const QSharedPointer<RowFileEvent> ConstPtrTo_RowFileEvent;
typedef const QSharedPointer<const RowFileEvent> ConstPtrToConst_RowFileEvent;

#endif // TABLEINFOFILEEVENT_H
