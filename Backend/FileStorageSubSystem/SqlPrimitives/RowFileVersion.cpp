#include "RowFileVersion.h"

#include "QuerySaveGroupItem.h"
#include "QueryFileVersion.h"
#include "QueryFileRecord.h"
#include "QueryFileEvent.h"
#include "RowInserter.h"

#include <QSqlQuery>
#include <QSqlRecord>

RowFileVersion::RowFileVersion() : BaseRow(TABLE_NAME_FILE_VERSION)
{
    this->parentRecordID = INVALID_FIELD_VALUE_QLONGLONG;
    this->versionNumber = INVALID_FIELD_VALUE_QLONGLONG;
    this->internalFileName = INVALID_FIELD_VALUE_QSTRING;
    this->size = INVALID_FIELD_VALUE_QLONGLONG;
    this->timestamp = INVALID_FIELD_VALUE_QDATETIME;
    this->description = INVALID_FIELD_VALUE_QSTRING;
    this->hash = INVALID_FIELD_VALUE_QSTRING;
}

RowFileVersion::RowFileVersion(const QSqlDatabase &db,
                               const QSqlRecord &record)
    : BaseRow(db, record.value(TABLE_FILE_VERSION_COLNAME_VERSION_ID).toLongLong(), TABLE_NAME_FILE_VERSION)
{
    this->parentRecordID = record.value(TABLE_FILE_VERSION_COLNAME_PARENT_RECORD_ID).toLongLong();
    this->versionNumber = record.value(TABLE_FILE_VERSION_COLNAME_VERSION_NUMBER).toLongLong();
    this->internalFileName = record.value(TABLE_FILE_VERSION_COLNAME_INTERNAL_FILE_NAME).toString();
    this->size = record.value(TABLE_FILE_VERSION_COLNAME_SIZE).toLongLong();
    this->timestamp = record.value(TABLE_FILE_VERSION_COLNAME_TIMESTAMP).toDateTime();
    this->hash = record.value(TABLE_FILE_VERSION_COLNAME_HASH).toString();

    if(record.value(TABLE_FILE_VERSION_COLNAME_DESCRIPTION).isNull())
        this->description = INVALID_FIELD_VALUE_QSTRING;
    else
        this->description = record.value(TABLE_FILE_VERSION_COLNAME_DESCRIPTION).toString();
}

qlonglong RowFileVersion::getParentRecordID() const
{
    return this->parentRecordID;
}

qlonglong RowFileVersion::getVersionNumber() const
{
    return this->versionNumber;
}

bool RowFileVersion::setVersionNumber(qlonglong newVersionNumber)
{
    if(!this->isExistInDB())
        return false;

    this->queryTemplateUpdateColumnValue<qlonglong>(TABLE_FILE_VERSION_COLNAME_VERSION_NUMBER, newVersionNumber);

    if(this->getLastError().type() == QSqlError::ErrorType::NoError)
    {
        this->versionNumber = newVersionNumber;
        return true;
    }

    return false;
}

const QString &RowFileVersion::getInternalFileName() const
{
    return internalFileName;
}

bool RowFileVersion::setInternalFileName(const QString &newInternaFilelName)
{
    if(!this->isExistInDB())
        return false;

    this->queryTemplateUpdateColumnValue<QString>(TABLE_FILE_VERSION_COLNAME_INTERNAL_FILE_NAME, newInternaFilelName);

    if(this->getLastError().type() == QSqlError::ErrorType::NoError)
    {
        this->internalFileName = newInternaFilelName;
        return true;
    }

    return false;
}

qlonglong RowFileVersion::getSize() const
{
    return this->size;
}

bool RowFileVersion::setSize(qlonglong newSize)
{
    if(!this->isExistInDB())
        return false;

    this->queryTemplateUpdateColumnValue<qlonglong>(TABLE_FILE_VERSION_COLNAME_SIZE, newSize);

    if(this->getLastError().type() == QSqlError::ErrorType::NoError)
    {
        this->size = newSize;
        return true;
    }

    return false;
}

const QDateTime &RowFileVersion::getTimestamp() const
{
    return this->timestamp;
}

bool RowFileVersion::setTimestamp(const QDateTime &newTimestamp)
{
    if(!this->isExistInDB())
        return false;

    this->queryTemplateUpdateColumnValue<QDateTime>(TABLE_FILE_VERSION_COLNAME_TIMESTAMP, newTimestamp);

    if(this->getLastError().type() == QSqlError::ErrorType::NoError)
    {
        this->timestamp = newTimestamp;
        return true;
    }

    return false;
}

const QString &RowFileVersion::getDescription() const
{
    return this->description;
}

bool RowFileVersion::setDescription(const QString &newDescription)
{
    if(!this->isExistInDB())
        return false;

    this->queryTemplateUpdateColumnValue<QString>(TABLE_FILE_VERSION_COLNAME_DESCRIPTION, newDescription);

    if(this->getLastError().type() == QSqlError::ErrorType::NoError)
    {
        this->description = newDescription;
        return true;
    }

    return false;
}

const QString &RowFileVersion::getHash() const
{
    return this->hash;
}

PtrTo_RowFileRecord RowFileVersion::getParentRowFileRecord() const
{
    QueryFileRecord queries(this->getDb());

    auto result = queries.selectRowByID(this->getParentRecordID());

    return result;
}

PtrTo_RowSaveGroupItem RowFileVersion::getSaveGroupItem() const
{
    auto result = QuerySaveGroupItem(this->getDb()).selectRowByParentFileVerisonID(this->getID());

    return result;
}

QString RowFileVersion::toString() const
{
    QString ptrStr = QString("0x%1").arg((quintptr)this, QT_POINTER_SIZE * 2, 16, QChar('0'));
    QString result = "RowFileVersion (%1) has:";
    result = result.arg(ptrStr);

    result += " " + TABLE_FILE_VERSION_COLNAME_PARENT_RECORD_ID + " = " + QString::number(this->getParentRecordID());
    result += " | " + TABLE_FILE_VERSION_COLNAME_VERSION_NUMBER + " = " + QString::number(this->getVersionNumber());
    result += " | " + TABLE_FILE_VERSION_COLNAME_INTERNAL_FILE_NAME + " = " + this->getInternalFileName();
    result += " | " + TABLE_FILE_VERSION_COLNAME_DESCRIPTION + " = " + this->getDescription();
    result += " | " + TABLE_FILE_VERSION_COLNAME_SIZE + " = " + QString::number(this->getSize());
    result += " | " + TABLE_FILE_VERSION_COLNAME_HASH + " = " + this->getHash();
    result += " | " + TABLE_FILE_VERSION_COLNAME_TIMESTAMP + " = " + this->getTimestamp().toString();

    return result;
}

bool RowFileVersion::setHash(const QString &newHash)
{
    if(!this->isExistInDB())
        return false;

    this->queryTemplateUpdateColumnValue<QString>(TABLE_FILE_VERSION_COLNAME_HASH, newHash);

    if(this->getLastError().type() == QSqlError::ErrorType::NoError)
    {
        this->hash = newHash;
        return true;
    }

    return false;
}

PtrTo_RowFileEvent RowFileVersion::markAsUnRegistered()
{
    auto *rowInserter = new RowInserter(this->getDb());
    ScopedPtrTo_RowFileEventInserter eventInserter(rowInserter);

    PtrTo_RowFileEvent result;
    result = eventInserter->insertUnRegisteredFileEvent(this->getInternalFileName());

    return result;
}

PtrTo_RowSaveGroupItem RowFileVersion::includeInSaveGroup(qlonglong saveGroupID)
{
    auto *rowInserter = new RowInserter(this->getDb());
    ScopedPtrTo_RowSaveGroupItemInserter sgInserter(rowInserter);

    PtrTo_RowSaveGroupItem result;
    result = sgInserter->insertSaveGroupItem(saveGroupID, this->getID(), this->getDescription());

    return result;
}

template<typename TypeColumnValue>
void RowFileVersion::queryTemplateUpdateColumnValue(const QString &columnName,
                                                    TypeColumnValue newColumnValue)
{
    QString queryTemplate = "UPDATE %1 "
                            "SET %2 = :2 "
                            "WHERE %3 = :3 AND %4 = :4;" ;

    queryTemplate = queryTemplate.arg(this->getTableName(),        // 1
                                      columnName,                  // 2
                                      TABLE_FILE_VERSION_COLNAME_PARENT_RECORD_ID,    // 3
                                      TABLE_FILE_VERSION_COLNAME_VERSION_NUMBER);     // 4


    QSqlQuery query(this->getDb());
    query.prepare(queryTemplate);

    query.bindValue(":2", newColumnValue);
    query.bindValue(":3", this->getParentRecordID());
    query.bindValue(":4", this->getVersionNumber());

    query.exec();
    this->setLastError(query.lastError());
}
