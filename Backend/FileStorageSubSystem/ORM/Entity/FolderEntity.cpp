#include "FolderEntity.h"

FolderEntity::FolderEntity()
{
    setIsExist(false);
    setPrimaryKey("");

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

QList<FileEntity> FolderEntity::getChildFiles() const
{
    return childFiles;
}

QString FolderEntity::getPrimaryKey() const
{
    return primaryKey;
}

void FolderEntity::setPrimaryKey(const QString &newPrimaryKey)
{
    primaryKey = newPrimaryKey;
}

void FolderEntity::setIsExist(bool newIsExist)
{
    _isExist = newIsExist;
}
