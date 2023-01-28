#include "FileEntity.h"

FileEntity::FileEntity()
{
    setIsExist(false);
    setPrimaryKey("");

    fileName = "";
    symbolFolderPath = "";
    isFrozen = false;
}

QString FileEntity::symbolFilePath() const
{
    return symbolFolderPath + fileName;
}

bool FileEntity::isExist() const
{
    return _isExist;
}

QList<FileVersionEntity> FileEntity::getVersionList() const
{
    return versionList;
}

QString FileEntity::getPrimaryKey() const
{
    return primaryKey;
}

void FileEntity::setPrimaryKey(const QString &newPrimaryKey)
{
    primaryKey = newPrimaryKey;
}

void FileEntity::setIsExist(bool newIsExist)
{
    _isExist = newIsExist;
}
