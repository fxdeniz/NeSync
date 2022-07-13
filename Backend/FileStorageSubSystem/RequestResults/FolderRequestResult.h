#ifndef FOLDERREQUESTRESULT_H
#define FOLDERREQUESTRESULT_H

#include "Backend/FileStorageSubSystem/SqlPrimitives/RowFolderRecord.h"
#include "FileStorageSubSystem/RequestResults/FileRequestResult.h"

#include <QIcon>

class FolderRequestResult
{
public:
    friend class FileStorageManager;

    bool isExist() const;

    const QString &folderName() const;
    const QString &directory() const;
    const QString &parentDirectory() const;
    bool isFavorite() const;
    const QList<FolderRequestResult> &childFolderList() const;
    const QList<FileRequestResult> &childFileList() const;

    const QIcon &folderIcon() const;

private:
    FolderRequestResult();
    FolderRequestResult(PtrTo_RowFolderRecord row);
    static FolderRequestResult leafFolderFrom(PtrTo_RowFolderRecord row);

private:
    bool exist;

    QString _folderName;
    QString _directory;
    QString _parentDirectory;

    bool _favorite;

    QList<FolderRequestResult> _childFolderList;
    QList<FileRequestResult> _childFileList;

    QIcon _icon;
};

#endif // FOLDERREQUESTRESULT_H
