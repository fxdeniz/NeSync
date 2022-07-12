#include "FileVersionMetaData.h"

#include "Backend/FileStorageSubSystem/SqlPrimitives/RowFileRecord.h"

FileVersionMetaData::FileVersionMetaData()
{
    this->exist = false;
    this->_symbolFilePathToParent = INVALID_FIELD_VALUE_QSTRING;
    this->_versionNumber = INVALID_FIELD_VALUE_QLONGLONG;
    this->_size = INVALID_FIELD_VALUE_QLONGLONG;
    this->_timestamp = INVALID_FIELD_VALUE_QDATETIME;
    this->_description = INVALID_FIELD_VALUE_QSTRING;
}

FileVersionMetaData::FileVersionMetaData(PtrTo_RowFileVersion row)
{
    this->exist = true;
    this->_symbolFilePathToParent = row->getParentRowFileRecord()->getSymbolFilePath();
    this->_versionNumber = row->getVersionNumber();
    this->_size = row->getSize();
    this->_timestamp = row->getTimestamp();
    this->_description = row->getDescription();
}

const QString &FileVersionMetaData::symbolFilePathToParent() const
{
    return _symbolFilePathToParent;
}

bool FileVersionMetaData::isExist() const
{
    return exist;
}

qlonglong FileVersionMetaData::versionNumber() const
{
    return _versionNumber;
}

qlonglong FileVersionMetaData::fileSize() const
{
    return _size;
}

const QDateTime &FileVersionMetaData::timestamp() const
{
    return _timestamp;
}

const QString &FileVersionMetaData::description() const
{
    return _description;
}
