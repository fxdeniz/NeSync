#include "FolderEntity.h"

FolderEntity::FolderEntity()
{
    setIsExist(false);

    parentFolderPath = "";
    suffixPath = "";
    userFolderPath = "";
    isFrozen = false;
}

QString FolderEntity::symbolFolderPath() const
{
    QString result = parentFolderPath + suffixPath;
    return result;
}

bool FolderEntity::isExist() const
{
    return _isExist;
}

QList<FolderEntity> FolderEntity::getChildFolders() const
{
    return childFolders;
}

void FolderEntity::setIsExist(bool newIsExist)
{
    _isExist = newIsExist;
}
