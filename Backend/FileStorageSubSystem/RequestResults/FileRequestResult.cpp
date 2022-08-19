#include "FileRequestResult.h"

#include <QFileIconProvider>

FileRequestResult::FileRequestResult()
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
    this->_latestVersionNumber = INVALID_FIELD_VALUE_QLONGLONG;
}

FileRequestResult::FileRequestResult(PtrTo_RowFileRecord row)
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

    QFileInfo info(row->getFileName() + row->getFileExtension());
    QFileIconProvider provider;
    this->_icon = provider.icon(info);

    this->_versionNumbers = row->getVersionNumbers();
    this->_latestVersionNumber = row->getLatestVersionNumber();
}

bool FileRequestResult::isExist() const
{
    return exist;
}

const QString &FileRequestResult::fileName() const
{
    return this->_fileName;
}

const QString &FileRequestResult::fileExtension() const
{
    return _fileExtension;
}

const QString &FileRequestResult::symbolDirectory() const
{
    return _symbolDirectory;
}

const QString &FileRequestResult::symbolFilePath() const
{
    return this->_symbolFilePath;
}

const QString &FileRequestResult::userDirectory() const
{
    return _userDirectory;
}

const QString &FileRequestResult::userFilePath() const
{
    return this->_userFilePath;
}

bool FileRequestResult::isFavorite() const
{
    return _favorite;
}

bool FileRequestResult::isFrozen() const
{
    return this->_frozen;
}

bool FileRequestResult::isAutoSyncEnabled() const
{
    return _autoSyncEnabled;
}

const QIcon &FileRequestResult::fileIcon() const
{
    return this->_icon;
}

QList<qlonglong> FileRequestResult::versionNumbers()
{
    return this->_versionNumbers;
}

qlonglong FileRequestResult::latestVersionNumber() const
{
    return _latestVersionNumber;
}
