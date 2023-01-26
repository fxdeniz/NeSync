#include "FileVersionEntity.h"

FileVersionEntity::FileVersionEntity()
{
    setIsExist(false);

    symbolFilePath = "";
    versionNumber = 0;
    internalFileName = "";
    size = 0;
    description = "";
    hash = "";
}

bool FileVersionEntity::isExist() const
{
    return _isExist;
}

QPair<QString, qlonglong> FileVersionEntity::getPrimaryKey() const
{
    return primaryKey;
}

void FileVersionEntity::setPrimaryKey(QString &symbolFilePath, qlonglong versionNumber)
{
    primaryKey.first = symbolFilePath;
    primaryKey.second = versionNumber;
}

void FileVersionEntity::setIsExist(bool newIsExist)
{
    _isExist = newIsExist;
}
