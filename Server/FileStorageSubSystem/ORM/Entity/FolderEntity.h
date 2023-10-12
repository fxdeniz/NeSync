#ifndef FOLDERENTITY_H
#define FOLDERENTITY_H

#include "FileEntity.h"

class FolderEntity
{
public:
    friend class FolderRepository;

    FolderEntity();

    QString suffixPath;
    QString parentFolderPath;
    QString userFolderPath;
    bool isFrozen;

    QString symbolFolderPath() const;
    bool isExist() const;

    QList<FolderEntity> getChildFolders() const;
    QList<FileEntity> getChildFiles() const;

    QString getPrimaryKey() const;

private:
    QList<FolderEntity> childFolders;
    QList<FileEntity> childFiles;

    void setPrimaryKey(const QString &newPrimaryKey);
    QString primaryKey;

    void setIsExist(bool newIsExist);
    bool _isExist;
};

#endif // FOLDERENTITY_H
