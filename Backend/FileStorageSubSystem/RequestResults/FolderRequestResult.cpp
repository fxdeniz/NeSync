#include "FolderRequestResult.h"
#include "Backend/FileStorageSubSystem/SqlPrimitives/RowFileRecord.h"

#include <QFileIconProvider>

FolderRequestResult::FolderRequestResult()
{
    exist = false;
    _folderName = INVALID_FIELD_VALUE_QSTRING;
    _directory = INVALID_FIELD_VALUE_QSTRING;
    _parentDirectory = INVALID_FIELD_VALUE_QSTRING;
    _favorite = INVALID_FIELD_VALUE_BOOL;
}

FolderRequestResult::FolderRequestResult(PtrTo_RowFolderRecord row)
{
    exist = true;
    _folderName = row->getSuffixDirectory();
    _directory = row->getDirectory();
    _parentDirectory = row->getParentDirectory();
    _favorite = row->getIsFavorite();

    for(const PtrTo_RowFolderRecord &childFolder : row->getAllChildRowFolderRecords())
        _childFolderList.append(FolderRequestResult::leafFolderFrom(childFolder));

    for(const PtrTo_RowFileRecord &childFile : row->getAllChildRowFileRecords())
        _childFileList.append(childFile);

    QFileIconProvider provider;
    _icon = provider.icon(QFileIconProvider::IconType::Folder);
}

FolderRequestResult FolderRequestResult::leafFolderFrom(PtrTo_RowFolderRecord row)
{
    FolderRequestResult result;
    result.exist = true;
    result._folderName = row->getSuffixDirectory();
    result._directory = row->getDirectory();
    result._parentDirectory = row->getParentDirectory();
    result._favorite = row->getIsFavorite();

    QFileIconProvider provider;
    result._icon = provider.icon(QFileIconProvider::IconType::Folder).pixmap(20, 20);

    return result;
}

const QIcon &FolderRequestResult::folderIcon() const
{
    return _icon;
}

bool FolderRequestResult::isExist() const
{
    return exist;
}

const QString &FolderRequestResult::directory() const
{
    return _directory;
}

const QString &FolderRequestResult::folderName() const
{
    return _folderName;
}

const QString &FolderRequestResult::parentDirectory() const
{
    return _parentDirectory;
}

bool FolderRequestResult::isFavorite() const
{
    return _favorite;
}

const QList<FileRequestResult> &FolderRequestResult::childFileList() const
{
    return _childFileList;
}

const QList<FolderRequestResult> &FolderRequestResult::childFolderList() const
{
    return _childFolderList;
}
