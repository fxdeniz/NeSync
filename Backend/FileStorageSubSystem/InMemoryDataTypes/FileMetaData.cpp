#include "FileMetaData.h"

#include <QFileIconProvider>

FileMetaData::FileMetaData()
{
    this->exist = false;
    this->_fileName = INVALID_FIELD_VALUE_QSTRING;
    this->_fileExtension = INVALID_FIELD_VALUE_QSTRING;
    this->_symbolDirectory = INVALID_FIELD_VALUE_QSTRING;
    this->_symbolFilePath = INVALID_FIELD_VALUE_QSTRING;
    this->_userDirectory = INVALID_FIELD_VALUE_QSTRING;
    this->_userFilePath = INVALID_FIELD_VALUE_QSTRING;
    this->_favorite = INVALID_FIELD_VALUE_BOOL;
    this->_frozen = INVALID_FIELD_VALUE_BOOL;
    this->_autoSyncEnabled = INVALID_FIELD_VALUE_BOOL;
}

FileMetaData::FileMetaData(PtrTo_RowFileRecord row)
{
    this->exist = true;
    this->_fileExtension = row->getFileExtension();
    this->_fileName = row->getFileName();
    this->_symbolDirectory = row->getSymbolDirectory();
    this->_symbolFilePath = row->getSymbolFilePath();
    this->_userDirectory = row->getUserDirectory();
    this->_userFilePath = row->getUserFilePath();
    this->_favorite = row->getIsFavorite();
    this->_frozen = row->getIsFrozen();
    this->_autoSyncEnabled = row->getIsAutoSyncEnabled();

    QFileInfo info(row->getSymbolDirectory());
    QFileIconProvider provider;
    this->_icon = provider.icon(info);

    this->_versionNumbers = row->getVersionNumbers();
}

bool FileMetaData::isExist() const
{
    return exist;
}

const QString &FileMetaData::fileName() const
{
    return this->_fileName;
}

const QString &FileMetaData::fileExtension() const
{
    return _fileExtension;
}

const QString &FileMetaData::symbolDirectory() const
{
    return _symbolDirectory;
}

const QString &FileMetaData::symbolFilePath() const
{
    return this->_symbolFilePath;
}

const QString &FileMetaData::userDirectory() const
{
    return _userDirectory;
}

const QString &FileMetaData::userFilePath() const
{
    return this->_userFilePath;
}

bool FileMetaData::isFavorite() const
{
    return _favorite;
}

bool FileMetaData::isFrozen() const
{
    return this->_frozen;
}

bool FileMetaData::isAutoSyncEnabled() const
{
    return _autoSyncEnabled;
}

const QIcon &FileMetaData::fileIcon() const
{
    return this->_icon;
}

QList<qlonglong> FileMetaData::versionNumbers()
{
    return this->_versionNumbers;
}
