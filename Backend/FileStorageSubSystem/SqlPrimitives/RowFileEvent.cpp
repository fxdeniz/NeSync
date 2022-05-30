#include "RowFileEvent.h"

#include <QSqlQuery>

RowFileEvent::RowFileEvent() : BaseRow(TABLE_NAME_FILE_EVENT)
{
    this->parentRecordID = INVALID_FIELD_VALUE_QLONGLONG;
    this->parentVersionID = INVALID_FIELD_VALUE_QLONGLONG;
    this->parentEventID = INVALID_FIELD_VALUE_QLONGLONG;
    this->type = NONEXIST_DEFAULT_EVENT_TYPE;
    this->status = INVALID_FIELD_VALUE_INT;
    this->timestamp = INVALID_FIELD_VALUE_QDATETIME;
    this->data = INVALID_FIELD_VALUE_QSTRING;
}

RowFileEvent::RowFileEvent(const QSqlDatabase &db,
                           const QSqlRecord &record)
    : BaseRow(db, record.value(TABLE_FILE_EVENT_COLNAME_EVENT_ID).toLongLong(), TABLE_NAME_FILE_EVENT)
{
    if(record.value(TABLE_FILE_EVENT_COLNAME_PARENT_RECORD_ID).isNull())
        this->parentRecordID = INVALID_FIELD_VALUE_QLONGLONG;
    else
        this->parentRecordID = record.value(TABLE_FILE_EVENT_COLNAME_PARENT_RECORD_ID).toLongLong();

    if(record.value(TABLE_FILE_EVENT_COLNAME_PARENT_VERSION_ID).isNull())
        this->parentVersionID = INVALID_FIELD_VALUE_QLONGLONG;
    else
        this->parentVersionID = record.value(TABLE_FILE_EVENT_COLNAME_PARENT_VERSION_ID).toLongLong();

    if(record.value(TABLE_FILE_EVENT_COLNAME_PARENT_EVENT_ID).isNull())
        this->parentEventID = INVALID_FIELD_VALUE_QLONGLONG;
    else
        this->parentEventID = record.value(TABLE_FILE_EVENT_COLNAME_PARENT_EVENT_ID).toLongLong();

    if(record.value(TABLE_FILE_EVENT_COLNAME_STATUS).isNull())
        this->status = INVALID_FIELD_VALUE_INT;
    else
        this->status = record.value(TABLE_FILE_EVENT_COLNAME_STATUS).toInt();

    if(record.value(TABLE_FILE_EVENT_COLNAME_DATA).isNull())
        this->data = INVALID_FIELD_VALUE_QSTRING;
    else
        this->data = record.value(TABLE_FILE_EVENT_COLNAME_DATA).toString();

    this->type = record.value(TABLE_FILE_EVENT_COLNAME_TYPE).value<FileEventType>();
    this->timestamp = record.value(TABLE_FILE_EVENT_COLNAME_TIMESTAMP).toDateTime();
}

RowFileEvent::RowFileEvent(const QSqlDatabase &db,
                           qlonglong eventID,
                           qlonglong parentRecordID,
                           qlonglong parentVersionID,
                           FileEventType type,
                           int status,
                           const QString data,
                           const QDateTime &timestamp) : BaseRow(db, eventID, TABLE_NAME_FILE_EVENT)
{
    this->parentRecordID = parentRecordID;
    this->parentVersionID = parentVersionID;
    this->type = type;
    this->status = status;
    this->data = data;
    this->timestamp = timestamp;
}

qlonglong RowFileEvent::getParentRecordID() const
{
    return parentRecordID;
}

bool RowFileEvent::setParentRecordID(qlonglong newParentRecordID)
{
    if(!this->isExistInDB())
        return false;

    this->queryTemplateUpdateColumnValue<qlonglong>(TABLE_FILE_EVENT_COLNAME_PARENT_RECORD_ID, newParentRecordID);

    if(this->getLastError().type() == QSqlError::ErrorType::NoError)
    {
        this->parentRecordID = newParentRecordID;

        return true;
    }

    return false;
}

qlonglong RowFileEvent::getParentVersionID() const
{
    return this->parentVersionID;
}

bool RowFileEvent::setParentVersionID(qlonglong newParentVersionNumber)
{
    if(!this->isExistInDB())
        return false;

    this->queryTemplateUpdateColumnValue<qlonglong>(TABLE_FILE_EVENT_COLNAME_PARENT_VERSION_ID, newParentVersionNumber);

    if(this->getLastError().type() == QSqlError::ErrorType::NoError)
    {
        this->parentVersionID = newParentVersionNumber;

        return true;
    }

    return false;
}

qlonglong RowFileEvent::getParentEventID() const
{
    return parentEventID;
}

bool RowFileEvent::setParentEventID(qlonglong newParentEventID)
{
    if(!this->isExistInDB())
        return false;

    this->queryTemplateUpdateColumnValue<qlonglong>(TABLE_FILE_EVENT_COLNAME_PARENT_EVENT_ID, newParentEventID);

    if(this->getLastError().type() == QSqlError::ErrorType::NoError)
    {
        this->parentEventID = newParentEventID;

        return true;
    }

    return false;
}

FileEventType RowFileEvent::getType() const
{
    return type;
}

bool RowFileEvent::setType(FileEventType newType)
{
    if(!this->isExistInDB())
        return false;

    this->queryTemplateUpdateColumnValue<FileEventType>(TABLE_FILE_EVENT_COLNAME_TYPE, newType);

    if(this->getLastError().type() == QSqlError::ErrorType::NoError)
    {
        this->type = newType;

        return true;
    }

    return false;
}

int RowFileEvent::getStatus() const
{
    return status;
}

bool RowFileEvent::setStatus(int newStatus)
{
    if(!this->isExistInDB())
        return false;

    this->queryTemplateUpdateColumnValue<int>(TABLE_FILE_EVENT_COLNAME_STATUS, newStatus);

    if(this->getLastError().type() == QSqlError::ErrorType::NoError)
    {
        this->status = newStatus;

        return true;
    }

    return false;
}

const QDateTime &RowFileEvent::getTimestamp() const
{
    return timestamp;
}

bool RowFileEvent::setTimestamp(const QDateTime &newTimestamp)
{
    if(!this->isExistInDB())
        return false;

    this->queryTemplateUpdateColumnValue<QDateTime>(TABLE_FILE_EVENT_COLNAME_TIMESTAMP, newTimestamp);

    if(this->getLastError().type() == QSqlError::ErrorType::NoError)
    {
        this->timestamp = newTimestamp;

        return true;
    }

    return false;
}

const QString &RowFileEvent::getData() const
{
    return data;
}

bool RowFileEvent::isUnRegisteredFileEvent() const
{
    if(this->getType() == FileEventType::UnRegisteredFileEvent)
        return true;

    return false;
}

QString RowFileEvent::toString() const
{
    QString ptrStr = QString("0x%1").arg((quintptr)this, QT_POINTER_SIZE * 2, 16, QChar('0'));
    QString result = "RowFileEvent (%1) has:";
    result = result.arg(ptrStr);

    result += " " + TABLE_FILE_EVENT_COLNAME_EVENT_ID + " = " + QString::number(this->getID());
    result += " | " + TABLE_FILE_EVENT_COLNAME_PARENT_RECORD_ID + " = " + QString::number(this->getParentRecordID());
    result += " | " + TABLE_FILE_EVENT_COLNAME_PARENT_EVENT_ID + " = " + QString::number(this->getParentEventID());
    result += " | " + TABLE_FILE_EVENT_COLNAME_TYPE + " = " + QString::number(this->getType());
    result += " | " + TABLE_FILE_EVENT_COLNAME_STATUS + " = " + QString::number(this->getStatus());
    result += " | " + TABLE_FILE_EVENT_COLNAME_DATA + " = " + this->getData();
    result += " | " + TABLE_FILE_EVENT_COLNAME_TIMESTAMP + " = " + this->getTimestamp().toString();

    return result;
}

bool RowFileEvent::setData(const QString &newData)
{
    if(!this->isExistInDB())
        return false;

    this->queryTemplateUpdateColumnValue<QString>(TABLE_FILE_EVENT_COLNAME_DATA, newData);

    if(this->getLastError().type() == QSqlError::ErrorType::NoError)
    {
        this->data = newData;

        return true;
    }

    return false;
}

template<typename TypeColumnValue>
void RowFileEvent::queryTemplateUpdateColumnValue(const QString &columnName,
                                                  TypeColumnValue newColumnValue)
{
    QString queryTemplate = "UPDATE %1 "
                            "SET %2 = :2 "
                            "WHERE %3 = :3;" ;

    queryTemplate = queryTemplate.arg(this->getTableName(),                   // 1
                                      columnName,                             // 2
                                      TABLE_FILE_EVENT_COLNAME_EVENT_ID);     // 3


    QSqlQuery query(this->getDb());
    query.prepare(queryTemplate);

    query.bindValue(":2", newColumnValue);
    query.bindValue(":3", this->getID());

    query.exec();
    this->setLastError(query.lastError());
}
