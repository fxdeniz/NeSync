#include "RowFolderRecord.h"

#include "QueryFolderRecord.h"
#include "QueryFileRecord.h"
#include "RowInserter.h"

#include <QSqlRecord>
#include <QSqlQuery>

RowFolderRecord::RowFolderRecord() : BaseRow(TABLE_NAME_FOLDER_RECORD)
{
    this->parentDirectory = INVALID_FIELD_VALUE_QSTRING;
    this->suffixDirectory = INVALID_FIELD_VALUE_QSTRING;
    this->userDirectory = INVALID_FIELD_VALUE_QSTRING;
}

RowFolderRecord::RowFolderRecord(const QSqlDatabase &db, const QSqlRecord &record)
    : BaseRow(db, record.value(TABLE_FOLDER_RECORD_COLNAME_FOLDER_ID).toLongLong(), TABLE_NAME_FOLDER_RECORD)
{
    if(record.value(TABLE_FOLDER_RECORD_COLNAME_PARENT_DIRECTORY).isNull())
    {
        this->parentDirectory = INVALID_FIELD_VALUE_QSTRING;
        this->userDirectory = INVALID_FIELD_VALUE_QSTRING;
    }
    else
    {
        this->parentDirectory = record.value(TABLE_FOLDER_RECORD_COLNAME_PARENT_DIRECTORY).toString();
        this->userDirectory = record.value(TABLE_FILE_RECORD_COLNAME_USER_DIRECTORY).toString();
    }

    this->suffixDirectory = record.value(TABLE_FOLDER_RECORD_COLNAME_SUFFIX_DIRECTORY).toString();
}

QString RowFolderRecord::getDirectory() const
{
    QString result = "";

    if(this->getParentDirectory() == INVALID_FIELD_VALUE_QSTRING)
        result = this->getSuffixDirectory();
    else
        result = this->getParentDirectory() + this->getSuffixDirectory();

    return result;
}

const QString &RowFolderRecord::getParentDirectory() const
{
    return this->parentDirectory;
}

bool RowFolderRecord::setParentDirectory(const QString &newParentDirectory)
{
    if(!this->isExistInDB())
        return false;

    this->queryTemplateUpdateColumnValue<QString>(TABLE_FOLDER_RECORD_COLNAME_PARENT_DIRECTORY, newParentDirectory);

    if(this->getLastError().type() == QSqlError::ErrorType::NoError)
    {
        this->parentDirectory = newParentDirectory;
        return true;
    }

    return false;
}

const QString &RowFolderRecord::getSuffixDirectory() const
{
    return this->suffixDirectory;
}

bool RowFolderRecord::setSuffixDirectory(const QString &newSuffixDirectory)
{
    if(!this->isExistInDB())
        return false;

    this->queryTemplateUpdateColumnValue<QString>(TABLE_FOLDER_RECORD_COLNAME_SUFFIX_DIRECTORY, newSuffixDirectory);

    if(this->getLastError().type() == QSqlError::ErrorType::NoError)
    {
        this->suffixDirectory = newSuffixDirectory;
        return true;
    }

    return false;
}

const QString &RowFolderRecord::getUserDirectory() const
{
    return this->userDirectory;
}

bool RowFolderRecord::setUserDirectory(const QString &newUserDirectory)
{
    if(!this->isExistInDB())
        return false;

    this->queryTemplateUpdateColumnValue<QString>(TABLE_FOLDER_RECORD_COLNAME_USER_DIRECTORY, newUserDirectory);

    if(this->getLastError().type() == QSqlError::ErrorType::NoError)
    {
        this->userDirectory = newUserDirectory;
        return true;
    }

    return false;
}

PtrTo_RowFolderRecord RowFolderRecord::getChildFolderBySuffix(const QString &suffixDir) const
{
    QueryFolderRecord queries(this->getDb());

    PtrTo_RowFolderRecord result = queries.selectRowByDirectory(this->getDirectory() + suffixDir);

    return result;
}

bool RowFolderRecord::getIsFavorite() const
{
    return isFavorite;
}

bool RowFolderRecord::setIsFavorite(bool newIsFavorite)
{
    if(!this->isExistInDB())
        return false;

    this->queryTemplateUpdateColumnValue<bool>(TABLE_FOLDER_RECORD_COLNAME_IS_FAVORITE, newIsFavorite);

    if(this->getLastError().type() == QSqlError::ErrorType::NoError)
    {
        this->isFavorite = newIsFavorite;
        return true;
    }

    return false;
}

QList<PtrTo_RowFileRecord> RowFolderRecord::getAllChildRowFileRecords() const
{
    QueryFileRecord queries(this->getDb());

    auto result = queries.selectRowsBySymbolDirectory(this->getDirectory());

    return result;
}

QList<PtrTo_RowFolderRecord> RowFolderRecord::getAllChildRowFolderRecords() const
{
    QueryFolderRecord queries(this->getDb());

    auto result = queries.selectRowsByParentDirectory(this->getDirectory());

    return result;
}

QList<PtrTo_RowFileRecord > RowFolderRecord::getChildRowFileRecordsByMatchingName(const QString &searchTerm) const
{
    QueryFileRecord queries(this->getDb());

    auto result = queries.selectRowsByMatchingFileNameFromDirectory(searchTerm, this->getDirectory());

    return result;
}

QList<PtrTo_RowFileRecord> RowFolderRecord::getChildRowFileRecordsByMatchingExtension(const QString &searchTerm) const
{
    QueryFileRecord queries(this->getDb());

    auto result = queries.selectRowsByMatchingFilExtensionFromDirectory(searchTerm, this->getDirectory());

    return result;
}

QString RowFolderRecord::toString() const
{
    QString ptrStr = QString("0x%1").arg((quintptr)this, QT_POINTER_SIZE * 2, 16, QChar('0'));
    QString result = "RowFolderRecord (%1) has:";
    result = result.arg(ptrStr);

    result += " " + TABLE_FOLDER_RECORD_COLNAME_FOLDER_ID + " = " + QString::number(this->getID());
    result += " | " + TABLE_FOLDER_RECORD_COLNAME_DIRECTORY + " = " + this->getDirectory();
    result += " | " + TABLE_FOLDER_RECORD_COLNAME_PARENT_DIRECTORY + " = " + this->getParentDirectory();
    result += " | " + TABLE_FOLDER_RECORD_COLNAME_SUFFIX_DIRECTORY + " = " + this->getSuffixDirectory();

    return result;
}

bool RowFolderRecord::addChildFolder(const QString &suffixDirectory)
{
    auto *rowInserter = new RowInserter(this->getDb());
    ScopedPtrTo_RowFolderRecordInserter folderInserter(rowInserter);

    auto childRow = folderInserter->insertChildFolder(this->getDirectory(), suffixDirectory);

    this->setLastError(folderInserter->getLastError());

    if(childRow->isExistInDB())
        return true;
    else
        return false;
}

template<typename TypeColumnValue>
void RowFolderRecord::queryTemplateUpdateColumnValue(const QString &columnName,
                                                     TypeColumnValue newColumnValue)
{
    QString queryTemplate = "UPDATE %1 "
                            "SET %2 = :2 "
                            "WHERE %3 = :3;" ;

    queryTemplate = queryTemplate.arg(this->getTableName(),                    // 1

                                      columnName,                              // 2
                                      TABLE_FOLDER_RECORD_COLNAME_FOLDER_ID);  // 3


    QSqlQuery query(this->getDb());
    query.prepare(queryTemplate);

    query.bindValue(":2", newColumnValue);
    query.bindValue(":3", this->getID());

    query.exec();
    this->setLastError(query.lastError());

}
