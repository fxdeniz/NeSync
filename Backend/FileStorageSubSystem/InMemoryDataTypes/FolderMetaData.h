#ifndef FOLDERMETADATA_H
#define FOLDERMETADATA_H

#include "Backend/FileStorageSubSystem/SqlPrimitives/RowFolderRecord.h"

class FolderMetaData
{
public:
    friend class FileStorageManager;

    FolderMetaData();

    bool isExist() const;

    const QString &folderName() const;
    const QString &directory() const;
    const QString &parentDirectory() const;
    bool isFavorite() const;
    const QList<QString> &childFolderList() const;
    const QList<QString> &symbolFilePathList() const;

private:
    FolderMetaData(PtrTo_RowFolderRecord row);

    bool exist;

    QString _folderName;
    QString _directory;
    QString _parentDirectory;

    bool _favorite;

    QList<QString> _childFolderList;
    QList<QString> _symbolFilePathList;
};

#endif // FOLDERMETADATA_H
