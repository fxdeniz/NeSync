#ifndef FOLDERREQUESTRESULT_H
#define FOLDERREQUESTRESULT_H

#include "Backend/FileStorageSubSystem/SqlPrimitives/RowFolderRecord.h"

#include <QIcon>

class FolderRequestResult
{
public:
    friend class FileStorageManager;

    FolderRequestResult();

    bool isExist() const;

    const QString &folderName() const;
    const QString &directory() const;
    const QString &parentDirectory() const;
    bool isFavorite() const;
    const QList<FolderRequestResult> &childFolderList() const;
    const QList<QString> &symbolFilePathList() const;

    const QIcon &folderIcon() const;

private:
    FolderRequestResult(PtrTo_RowFolderRecord row);
    static FolderRequestResult leafFrom(PtrTo_RowFolderRecord row);

private:
    bool exist;

    QString _folderName;
    QString _directory;
    QString _parentDirectory;

    bool _favorite;

    QList<FolderRequestResult> _childFolderList;
    QList<QString> _symbolFilePathList;

    QIcon _icon;
};

#endif // FOLDERREQUESTRESULT_H
