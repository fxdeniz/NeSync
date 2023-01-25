#include "FileEntity.h"

FileEntity::FileEntity()
{
    setIsExist(false);

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

void FileEntity::setIsExist(bool newIsExist)
{
    _isExist = newIsExist;
}
