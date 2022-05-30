#ifndef ROWFILEEVENT_H
#define ROWFILEEVENT_H

#include "BaseRow.h"
#include"TableInfoFileEvent.h"

#include <QSqlRecord>

class RowFileEvent : public BaseRow
{
public:
    friend class QueryFileEvent;

    RowFileEvent();

    qlonglong getEventID() const;
    qlonglong getParentRecordID() const;
    qlonglong getParentVersionID() const;
    qlonglong getParentEventID() const;
    FileEventType getType() const;
    int getStatus() const;
    const QDateTime &getTimestamp() const;
    const QString &getData() const;
    bool isUnRegisteredFileEvent() const;

    virtual QString toString() const override;

    bool setParentRecordID(qlonglong newParentRecordID);
    bool setParentVersionID(qlonglong newParentVersionNumber);
    bool setParentEventID(qlonglong newParentEventID);
    bool setType(FileEventType newType);
    bool setStatus(int newStatus);
    bool setTimestamp(const QDateTime &newTimestamp);
    bool setData(const QString &newData);

private:
    RowFileEvent(const QSqlDatabase &db, const QSqlRecord &record);
    RowFileEvent(const QSqlDatabase &db,
                 qlonglong eventID,
                 qlonglong parentRecordID,
                 qlonglong parentVersionID,
                 FileEventType type,
                 int status,
                 const QString data = "",
                 const QDateTime &timestamp = QDateTime::currentDateTime());

    qlonglong parentRecordID;
    qlonglong parentVersionID;
    qlonglong parentEventID;
    FileEventType type;
    int status;
    QDateTime timestamp;
    QString data;

    template<typename TypeColumnValue>
    void queryTemplateUpdateColumnValue(const QString &columnName,
                                        TypeColumnValue newColumnValue);

};

#endif // ROWFILEEVENT_H
