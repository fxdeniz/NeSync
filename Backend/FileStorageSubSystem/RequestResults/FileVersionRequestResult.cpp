#include "FileVersionRequestResult.h"

#include "Backend/FileStorageSubSystem/SqlPrimitives/RowFileRecord.h"

FileVersionRequestResult::FileVersionRequestResult()
{
    this->exist = false;
    this->_symbolFilePathToParent = INVALID_FIELD_VALUE_QSTRING;
    this->_versionNumber = INVALID_FIELD_VALUE_QLONGLONG;
    this->_size = INVALID_FIELD_VALUE_QLONGLONG;
    this->_timestamp = INVALID_FIELD_VALUE_QDATETIME;
    this->_description = INVALID_FIELD_VALUE_QSTRING;
}

FileVersionRequestResult::FileVersionRequestResult(PtrTo_RowFileVersion row)
{
    this->exist = true;
    this->_symbolFilePathToParent = row->getParentRowFileRecord()->getSymbolFilePath();
    this->_versionNumber = row->getVersionNumber();
    this->_size = row->getSize();
    this->_timestamp = row->getTimestamp();
    this->_description = row->getDescription();
}

const QString &FileVersionRequestResult::symbolFilePathToParent() const
{
    return _symbolFilePathToParent;
}

bool FileVersionRequestResult::isExist() const
{
    return exist;
}

qlonglong FileVersionRequestResult::versionNumber() const
{
    return _versionNumber;
}

qlonglong FileVersionRequestResult::fileSize() const
{
    return _size;
}

const QDateTime &FileVersionRequestResult::timestamp() const
{
    return _timestamp;
}

const QString &FileVersionRequestResult::description() const
{
    return _description;
}
