#include "RowFileRecord.h"

#include "QueryFileVersion.h"
#include "QueryFileEvent.h"
#include "RowInserter.h"

#include <QSqlQuery>

RowFileRecord::RowFileRecord() : BaseRow(TABLE_NAME_FILE_RECORD)
{
    this->fileName = INVALID_FIELD_VALUE_QSTRING;
    this->fileExtension = INVALID_FIELD_VALUE_QSTRING;
    this->symbolDirectory = INVALID_FIELD_VALUE_QSTRING;
    this->userDirectory = INVALID_FIELD_VALUE_QSTRING;
    this->isFavorite = INVALID_FIELD_VALUE_BOOL;
    this->isFrozen = INVALID_FIELD_VALUE_BOOL;
    this->isAutoSyncEnabled = INVALID_FIELD_VALUE_BOOL;
}

RowFileRecord::RowFileRecord(const QSqlDatabase &db, const QSqlRecord &record)
    : BaseRow(db, record.value(TABLE_FILE_RECORD_COLNAME_RECORD_ID).toLongLong(), TABLE_NAME_FILE_RECORD)
{
    this->fileName = record.value(TABLE_FILE_RECORD_COLNAME_FILE_NAME).toString();
    this->fileExtension = record.value(TABLE_FILE_RECORD_COLNAME_FILE_EXTENSION).toString();
    this->symbolDirectory = record.value(TABLE_FILE_RECORD_COLNAME_SYMBOL_DIRECTORY).toString();
    this->isFavorite = record.value(TABLE_FILE_RECORD_COLNAME_IS_FAVORITE).toBool();
    this->isFrozen = record.value(TABLE_FILE_RECORD_COLNAME_IS_FROZEN).toBool();
    this->isAutoSyncEnabled = record.value(TABLE_FILE_RECORD_COLNAME_IS_AUTO_SYNC_ENABLED).toBool();

    if(record.value(TABLE_FILE_RECORD_COLNAME_USER_DIRECTORY).isNull())
        this->userDirectory = INVALID_FIELD_VALUE_QSTRING;
    else
        this->userDirectory = record.value(TABLE_FILE_RECORD_COLNAME_USER_DIRECTORY).toString();
}

PtrTo_RowFileVersion RowFileRecord::insertVersion(qlonglong versionNumber,
                                            const QString &internalFileName,
                                            qlonglong size,
                                            const QString &hash,
                                            const QString &description)
{
    auto *rowInserter = new RowInserter(this->getDb());
    ScopedPtrTo_RowFileVersionInserter versionInserter(rowInserter);

    auto result = versionInserter->insertFileVersion(this->getID(), versionNumber, internalFileName, size, hash, description);
    this->setLastError(result->getLastError());

    return result;
}

const QString &RowFileRecord::getFileName() const
{
    return this->fileName;
}

bool RowFileRecord::setFileName(const QString &newFileName)
{
    if(!this->isExistInDB())
        return false;

    this->queryTemplateUpdateColumnValue<QString>(TABLE_FILE_RECORD_COLNAME_FILE_NAME, newFileName);

    if(this->getLastError().type() == QSqlError::ErrorType::NoError)
    {
        this->fileName = newFileName;

        return true;
    }

    return false;
}

const QString &RowFileRecord::getFileExtension() const
{
    return this->fileExtension;
}

bool RowFileRecord::setFileExtension(const QString &newFileExtension)
{
    if(!this->isExistInDB())
        return false;

    this->queryTemplateUpdateColumnValue<QString>(TABLE_FILE_RECORD_COLNAME_FILE_EXTENSION, newFileExtension);

    if(this->getLastError().type() == QSqlError::ErrorType::NoError)
    {
        this->fileExtension = newFileExtension;

        return true;
    }

    return false;
}

const QString &RowFileRecord::getSymbolDirectory() const
{
    return this->symbolDirectory;
}

QString RowFileRecord::getSymbolFilePath() const
{
    auto result = this->getSymbolDirectory() + this->getFileName() + this->getFileExtension();

    return result;
}

bool RowFileRecord::setSymbolDirectory(const QString &newSymbolDirectory)
{
    if(!this->isExistInDB())
        return false;

    this->queryTemplateUpdateColumnValue<QString>(TABLE_FILE_RECORD_COLNAME_SYMBOL_DIRECTORY, newSymbolDirectory);

    if(this->getLastError().type() == QSqlError::ErrorType::NoError)
    {
        this->symbolDirectory = newSymbolDirectory;

        return true;
    }

    return false;
}

const QString &RowFileRecord::getUserDirectory() const
{
    return this->userDirectory;
}

QString RowFileRecord::getUserFilePath() const
{
    QString result = "";

    if(this->getUserDirectory() == INVALID_FIELD_VALUE_QSTRING)
        result = INVALID_FIELD_VALUE_QSTRING;
    else
        result = this->getUserDirectory() + this->getFileName() + this->getFileExtension();

    return result;
}

bool RowFileRecord::setUserDirectory(const QString &newUserDirectory)
{
    if(!this->isExistInDB())
        return false;

    this->queryTemplateUpdateColumnValue<QString>(TABLE_FILE_RECORD_COLNAME_USER_DIRECTORY, newUserDirectory);

    if(this->getLastError().type() == QSqlError::ErrorType::NoError)
    {
        this->userDirectory = newUserDirectory;

        return true;
    }

    return false;
}

bool RowFileRecord::getIsFavorite() const
{
    return isFavorite;
}

bool RowFileRecord::setIsFavorite(bool newIsFavorite)
{
    if(!this->isExistInDB())
        return false;

    this->queryTemplateUpdateColumnValue<bool>(TABLE_FILE_RECORD_COLNAME_IS_FAVORITE, newIsFavorite);

    if(this->getLastError().type() == QSqlError::ErrorType::NoError)
    {
        this->isFavorite = newIsFavorite;

        return true;
    }

    return false;
}

bool RowFileRecord::getIsFrozen() const
{
    return this->isFrozen;
}

bool RowFileRecord::setIsFrozen(bool newIsFrozen)
{
    if(!this->isExistInDB())
        return false;

    this->queryTemplateUpdateColumnValue<bool>(TABLE_FILE_RECORD_COLNAME_IS_FROZEN, newIsFrozen);

    if(this->getLastError().type() == QSqlError::ErrorType::NoError)
    {
        this->isFrozen = newIsFrozen;

        return true;
    }

    return false;
}

bool RowFileRecord::getIsAutoSyncEnabled() const
{
    return this->isAutoSyncEnabled;
}

bool RowFileRecord::setIsAutoSyncEnabled(bool newIsAutoSyncEnabled)
{
    if(!this->isExistInDB())
        return false;

    this->queryTemplateUpdateColumnValue<bool>(TABLE_FILE_RECORD_COLNAME_IS_AUTO_SYNC_ENABLED, newIsAutoSyncEnabled);

    if(this->getLastError().type() == QSqlError::ErrorType::NoError)
    {
        this->isAutoSyncEnabled = newIsAutoSyncEnabled;

        return true;
    }

    return false;
}

qlonglong RowFileRecord::getRowFileVersionCount() const
{
    QueryFileVersion queries(this->getDb());

    auto result = queries.selectRowFileVersionCount(this->getID());

    return result;
}

qlonglong RowFileRecord::getLatestVersionNumber() const
{
    QueryFileVersion queries(this->getDb());

    auto result = queries.selectLatestVersionNumber(this->getID());

    return result;
}

PtrTo_RowFileVersion RowFileRecord::getRowFileVersion(qlonglong versionNumber) const
{
    QueryFileVersion queries(this->getDb());

    auto result = queries.selectRowByCompositeKey(this->getID(), versionNumber);

    return result;
}

QList<qlonglong> RowFileRecord::getVersionNumbers() const
{
    QueryFileVersion queries(this->getDb());

    auto result = queries.selectVersionNumbersOfParentRecord(this->getID());

    return result;
}

QList<PtrTo_RowFileVersion> RowFileRecord::getAllRowFileVersions() const
{
    QueryFileVersion queries(this->getDb());

    auto result = queries.selectAllVersionsOfParentRecord(this->getID());

    return result;
}

QString RowFileRecord::toString() const
{
    QString ptrStr = QString("0x%1").arg((quintptr)this, QT_POINTER_SIZE * 2, 16, QChar('0'));
    QString result = "RowFileRecord (%1) has:";
    result = result.arg(ptrStr);

    result += " " + TABLE_FILE_RECORD_COLNAME_RECORD_ID + " = " + QString::number(this->getID());
    result += " | " + TABLE_FILE_RECORD_COLNAME_FILE_NAME + " = " + this->getFileName();
    result += " | " + TABLE_FILE_RECORD_COLNAME_FILE_EXTENSION + " = " + this->getFileExtension();
    result += " | " + TABLE_FILE_RECORD_COLNAME_SYMBOL_DIRECTORY + " = " + this->getSymbolDirectory();
    result += " | " + TABLE_FILE_RECORD_COLNAME_USER_DIRECTORY + " = " + this->getUserDirectory();
    result += " | " + TABLE_FILE_RECORD_COLNAME_IS_FROZEN + " = " + QString::number(this->getIsFrozen());

    return result;
}

template<typename TypeColumnValue>
void RowFileRecord::queryTemplateUpdateColumnValue(const QString &columnName,
                                                   TypeColumnValue newColumnValue)
{
    QString queryTemplate = "UPDATE %1 "
                            "SET %2 = :2 "
                            "WHERE %3 = :3;" ;

    queryTemplate = queryTemplate.arg(this->getTableName(),                   // 1
                                      columnName,                             // 2
                                      TABLE_FILE_RECORD_COLNAME_RECORD_ID);   // 3


    QSqlQuery query(this->getDb());
    query.prepare(queryTemplate);

    query.bindValue(":2", newColumnValue);
    query.bindValue(":3", this->getID());

    query.exec();
    this->setLastError(query.lastError());
}
