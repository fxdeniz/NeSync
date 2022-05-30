#include "SaveGroupItemMetaData.h"

#include "Backend/FileStorageSubSystem/SqlPrimitives/RowFolderRecord.h"
#include "Backend/FileStorageSubSystem/SqlPrimitives/RowFileRecord.h"

SaveGroupItemMetaData::SaveGroupItemMetaData()
{
    this->exist = false;
    this->_symbolPathToParentFile = INVALID_FIELD_VALUE_QSTRING;
    this->_parentVersionNumber = INVALID_FIELD_VALUE_QLONGLONG;
    this->_saveGroupNumber = INVALID_FIELD_VALUE_QLONGLONG;
    this->_originalTimestamp = INVALID_FIELD_VALUE_QDATETIME;
    this->_originalDescription = INVALID_FIELD_VALUE_QSTRING;
}

SaveGroupItemMetaData::SaveGroupItemMetaData(PtrTo_RowSaveGroupItem row)
{
    this->exist = true;
    this->_symbolPathToParentFile = row->getParentRowFileVersion()->getParentRowFileRecord()->getSymbolFilePath();
    this->_parentVersionNumber = row->getParentVersionID();
    this->_saveGroupNumber = row->getSaveGroupID();
    this->_originalTimestamp = row->getOriginalTimestamp();
    this->_originalDescription = row->getOriginalDescription();
}

bool SaveGroupItemMetaData::isExist() const
{
    return this->exist;
}

const QString &SaveGroupItemMetaData::symbolPathToParentFile() const
{
    return _symbolPathToParentFile;
}

qlonglong SaveGroupItemMetaData::parentVersionNumber() const
{
    return _parentVersionNumber;
}

qlonglong SaveGroupItemMetaData::saveGroupNumber() const
{
    return _saveGroupNumber;
}

const QDateTime &SaveGroupItemMetaData::originalTimestamp() const
{
    return _originalTimestamp;
}

const QString &SaveGroupItemMetaData::originalDescription() const
{
    return _originalDescription;
}

QString SaveGroupItemMetaData::toString() const
{
    QString ptrStr = QString("0x%1").arg((quintptr)this, QT_POINTER_SIZE * 2, 16, QChar('0'));

    QString result = "SaveGroupItemInfo (%1) has:";
    result = result.arg(ptrStr);

    result += " pathToSymbolFile = " + this->symbolPathToParentFile();
    result += " | parentVersionNumber = " + QString::number(this->parentVersionNumber());
    result += " | saveGroupNumber = " + QString::number(this->saveGroupNumber());
    result += " | originalTimestamp = " + this->originalTimestamp().toString();
    result += " | originialDescription = " + this->originalDescription();

    return result;
}
