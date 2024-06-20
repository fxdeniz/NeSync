#ifndef FILEENTITY_H
#define FILEENTITY_H

#include "FileVersionEntity.h"

#include <QList>

class FileEntity
{
public:
    friend class FileRepository;
    friend class FolderRepository;

    FileEntity();

    QString fileName;
    QString symbolFolderPath;
    bool isFrozen;

    QString symbolFilePath() const;

    bool isExist() const;

    QList<FileVersionEntity> getVersionList() const;

    QString getPrimaryKey() const;

private:
    QList<FileVersionEntity> versionList;

    void setPrimaryKey(const QString &newPrimaryKey);
    QString primaryKey;

    void setIsExist(bool newIsExist);
    bool _isExist;
};

#endif // FILEENTITY_H
