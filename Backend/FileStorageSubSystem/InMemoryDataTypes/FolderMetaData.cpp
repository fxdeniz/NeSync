#include "FolderMetaData.h"
#include "Backend/FileStorageSubSystem/SqlPrimitives/RowFileRecord.h"

FolderMetaData::FolderMetaData()
{
    this->exist = false;
    this->_folderName = INVALID_FIELD_VALUE_QSTRING;
    this->_directory = INVALID_FIELD_VALUE_QSTRING;
    this->_parentDirectory = INVALID_FIELD_VALUE_QSTRING;
    this->_favorite = INVALID_FIELD_VALUE_BOOL;
}

FolderMetaData::FolderMetaData(PtrTo_RowFolderRecord row)
{
    this->exist = true;
    this->_folderName = row->getSuffixDirectory();
    this->_directory = row->getDirectory();
    this->_parentDirectory = row->getParentDirectory();
    this->_favorite = row->getIsFavorite();

    for(auto const childFolder : row->getAllChildRowFolderRecords())
        this->_childFolderList.append(childFolder->getDirectory());

    for(auto const childFile : row->getAllChildRowFileRecords())
        this->_symbolFilePathList.append(childFile->getSymbolFilePath());
}

bool FolderMetaData::isExist() const
{
    return this->exist;
}

const QString &FolderMetaData::directory() const
{
    return this->_directory;
}

const QString &FolderMetaData::folderName() const
{
    return this->_folderName;
}

const QString &FolderMetaData::parentDirectory() const
{
    return this->_parentDirectory;
}

bool FolderMetaData::isFavorite() const
{
    return _favorite;
}

const QList<QString> &FolderMetaData::symbolFilePathList() const
{
    return this->_symbolFilePathList;
}

const QList<QString> &FolderMetaData::childFolderList() const
{
    return this->_childFolderList;
}
