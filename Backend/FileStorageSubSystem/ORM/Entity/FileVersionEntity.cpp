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

void FileVersionEntity::setIsExist(bool newIsExist)
{
    _isExist = newIsExist;
}
