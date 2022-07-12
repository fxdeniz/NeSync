#include "FolderMetaData.h"
#include "Backend/FileStorageSubSystem/SqlPrimitives/RowFileRecord.h"

#include <QFileIconProvider>

FolderMetaData::FolderMetaData()
{
    exist = false;
    _folderName = INVALID_FIELD_VALUE_QSTRING;
    _directory = INVALID_FIELD_VALUE_QSTRING;
    _parentDirectory = INVALID_FIELD_VALUE_QSTRING;
    _favorite = INVALID_FIELD_VALUE_BOOL;
}

FolderMetaData::FolderMetaData(PtrTo_RowFolderRecord row)
{
    exist = true;
    _folderName = row->getSuffixDirectory();
    _directory = row->getDirectory();
    _parentDirectory = row->getParentDirectory();
    _favorite = row->getIsFavorite();

    for(auto const childFolder : row->getAllChildRowFolderRecords())
        _childFolderList.append(childFolder->getDirectory());

    for(auto const childFile : row->getAllChildRowFileRecords())
        _symbolFilePathList.append(childFile->getSymbolFilePath());

    QFileIconProvider provider;
    _icon = provider.icon(QFileIconProvider::IconType::Folder).pixmap(20, 20);
}

const QIcon &FolderMetaData::folderIcon() const
{
    return _icon;
}

bool FolderMetaData::isExist() const
{
    return exist;
}

const QString &FolderMetaData::directory() const
{
    return _directory;
}

const QString &FolderMetaData::folderName() const
{
    return _folderName;
}

const QString &FolderMetaData::parentDirectory() const
{
    return _parentDirectory;
}

bool FolderMetaData::isFavorite() const
{
    return _favorite;
}

const QList<QString> &FolderMetaData::symbolFilePathList() const
{
    return _symbolFilePathList;
}

const QList<QString> &FolderMetaData::childFolderList() const
{
    return _childFolderList;
}
