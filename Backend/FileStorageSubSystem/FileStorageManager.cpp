#include "FileStorageManager.h"

#include "SqlPrimitives/QuerySaveGroupItem.h"
#include "SqlPrimitives/QueryFolderRecord.h"
#include "SqlPrimitives/QueryFileVersion.h"
#include "SqlPrimitives/QueryFileRecord.h"
#include "SqlPrimitives/QueryFileEvent.h"
#include "SqlPrimitives/RowInserter.h"
#include "SqlPrimitives/RowDeleter.h"

#include <QCryptographicHash>
#include <QStandardPaths>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QDateTime>
#include <QUuid>
#include <QDir>

const QString FileStorageManager::DB_FILE_NAME = "ff_database.db3";
const QString FileStorageManager::INTERNAL_FILE_NAME_EXTENSION = ".file";
const QString FileStorageManager::CONST_SYMBOL_DIRECTORY_SEPARATOR = "/";

FileStorageManager::FileStorageManager(const QString &backupDirectory)
{
    this->backupDirectory = backupDirectory;
    QString connectionName = QUuid::createUuid().toString(QUuid::StringFormat::Id128);
    this->extractSqliteDBIfNotExist();

    this->db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    QString dbFilePath = getBackupDirectory() + DB_FILE_NAME;
    this->db.setDatabaseName(dbFilePath);
    this->db.open();
    this->db.exec("PRAGMA foreign_keys = ON;");
    this->db.exec("PRAGMA synchronous = FULL;");

    auto *rowInserter = new RowInserter(this->db);
    ScopedPtrTo_RowFolderRecordInserter folderInserter(rowInserter);
    folderInserter->insertRootFolder(CONST_SYMBOL_DIRECTORY_SEPARATOR);

    QuerySaveGroupItem queries(this->db);
    this->currentSaveGroupNumber = queries.selectCurrentSaveGroupID();
}

QSharedPointer<FileStorageManager> FileStorageManager::instance()
{
    auto appDataDir = QStandardPaths::writableLocation(QStandardPaths::StandardLocation::TempLocation);
    appDataDir = QDir::toNativeSeparators(appDataDir);
    appDataDir += QDir::separator();

    auto backupDir = appDataDir + "backup" + QDir::separator();

    QDir dir;
    dir.mkdir(backupDir);

    auto *rawPtr = new FileStorageManager(backupDir);
    QSharedPointer<FileStorageManager> result(rawPtr);

    return result;
}

bool FileStorageManager::addNewFile(const QString &pathToFile,
                                    const QString &symbolDirectory,
                                    bool isFrozenFile,
                                    bool isAutoSyncEnabled,
                                    const QString &userDirectory,
                                    const QString &description,
                                    const QString &newFileName)
{
    QString fileName, fileExtension, internalFileName, fileHash;
    qlonglong fileSize;

    PtrTo_RowFileEvent rowUnRegisteredFileEvent = this->addUnRegisteredFile(pathToFile,
                                                                      &internalFileName,
                                                                      &fileHash,
                                                                      &fileSize);

    if(!rowUnRegisteredFileEvent->isExistInDB())
        return false;

    bool isFolderCreated = this->addNewFolder(symbolDirectory);

    if(!isFolderCreated)
        return false;

    QFileInfo fileInfo(pathToFile);

    if(newFileName.isEmpty())
        fileName = fileInfo.baseName();
    else
        fileName = newFileName;

    if(fileInfo.completeSuffix().isEmpty())
        fileExtension = "";
    else
        fileExtension = ("." + fileInfo.completeSuffix());

    bool result = this->transactionInsertRowFileRecord(fileName,
                                                       fileExtension,
                                                       symbolDirectory,
                                                       userDirectory,
                                                       isFrozenFile,
                                                       isAutoSyncEnabled,
                                                       fileSize,
                                                       fileHash,
                                                       description,
                                                       internalFileName,
                                                       rowUnRegisteredFileEvent);

    if(result == true && isFrozenFile == false)
    {
        QString userFilePath = userDirectory + fileName + fileExtension;
        //emit signalMonitoredFileAddedByBackend(userFilePath);
    }

    return result;
}

bool FileStorageManager::addNewVersion(const QString &pathToSourceFile,
                                       const QString &pathToSymbolFile,
                                       qlonglong versionNumber,
                                       const QString &description)
{
    QString internalFileName, fileHash;
    qlonglong fileSize;

    PtrTo_RowFileEvent rowUnRegisteredFileEvent = this->addUnRegisteredFile(pathToSourceFile,
                                                                      &internalFileName,
                                                                      &fileHash,
                                                                      &fileSize);

    if(!rowUnRegisteredFileEvent->isExistInDB())
        return false;

    bool result = this->transactionInsertRowFileVersion(pathToSymbolFile,
                                                        versionNumber,
                                                        internalFileName,
                                                        fileSize,
                                                        fileHash,
                                                        description,
                                                        rowUnRegisteredFileEvent);

    return result;
}

bool FileStorageManager::appendNewVersion(const QString &pathToSourceFile,
                                          const QString &pathToSymbolFile,
                                          const QString &description)
{
    auto parentRecord = QueryFileRecord(this->db).selectRowBySymbolFilePath(pathToSymbolFile);

    if(!parentRecord->isExistInDB())
        return false;

    qlonglong newVersionNumber = parentRecord->getLatestVersionNumber() + 1;

    auto result = this->addNewVersion(pathToSourceFile, pathToSymbolFile, newVersionNumber, description);

    return result;
}

bool FileStorageManager::deleteFiles(const QStringList &symbolFilePathList)
{
    if(symbolFilePathList.isEmpty())
        return false;

    QList<PtrTo_RowFileRecord> argument;
    QStringList userFilePathList;

    for(const auto &currentSymbolPath : symbolFilePathList)
    {
        PtrTo_RowFileRecord targetRow = QueryFileRecord(this->db).selectRowBySymbolFilePath(currentSymbolPath);

        if(!targetRow->isExistInDB())
            return false;

        userFilePathList << targetRow->getUserFilePath();
        argument << targetRow;
    }

    QList<PtrTo_RowFileEvent> events = this->transactionDeleteRowFileRecord(argument);

    if(!events.isEmpty())
    {
//        for(const auto currentUserFilePath : userFilePathList)
//        {
//            emit signalMonitoredFileRemovedByBackend(currentUserFilePath);
//        }

        for(const auto currentEvent : events)
        {
            bool isFileDeleted = this->deleteUnRegisteredFile(currentEvent);

            if(!isFileDeleted)
                return false;
        }

        return true;
    }

    return false;
}

bool FileStorageManager::deleteVersions(const QString &pathToSymbolFile, QList<qlonglong> versionNumberList)
{
    if(versionNumberList.isEmpty())
        return false;

    PtrTo_RowFileRecord rowRecord = QueryFileRecord(this->db).selectRowBySymbolFilePath(pathToSymbolFile);

    if(!rowRecord->isExistInDB())
        return false;

    QList<PtrTo_RowFileVersion> argument;

    for(auto currentVersionNumber : versionNumberList)
    {
        PtrTo_RowFileVersion rowVersion = rowRecord->getRowFileVersion(currentVersionNumber);

        if(!rowVersion->isExistInDB())
            return false;

        argument << rowVersion;
    }

    QList<PtrTo_RowFileEvent> events = this->transactionDeleteRowFileVersion(argument);

    if(!events.isEmpty())
    {
        for(const auto currentEvent : events)
        {
            bool isFileDeleted = this->deleteUnRegisteredFile(currentEvent);

            if(!isFileDeleted)
                return false;
        }

        return true;
    }

    return false;
}

bool FileStorageManager::updateNameOfFile(const QString &pathToSymbolFile, const QString &newName)
{
    PtrTo_RowFileRecord rowRecord = QueryFileRecord(this->db).selectRowBySymbolFilePath(pathToSymbolFile);

    if(!rowRecord->isExistInDB())
        return false;

    if(rowRecord->getFileName() == newName)
        return true;

    auto oldUserFilePath = rowRecord->getUserFilePath();

    bool result = rowRecord->setFileName(newName);

//    if(result == true)
//        emit signalMonitoredFilePathChangedByBackend(oldUserFilePath, rowRecord->getUserFilePath());

    return result;
}

bool FileStorageManager::updateExtensionOfFile(const QString &pathToSymbolFile, const QString &newExtension)
{
    PtrTo_RowFileRecord rowRecord = QueryFileRecord(this->db).selectRowBySymbolFilePath(pathToSymbolFile);

    if(!rowRecord->isExistInDB())
        return false;

    if(rowRecord->getFileExtension() == newExtension)
        return true;

    auto oldUserFilePath = rowRecord->getUserFilePath();
    bool result = rowRecord->setFileExtension(newExtension);

//    if(result == true)
//        emit signalMonitoredFilePathChangedByBackend(oldUserFilePath, rowRecord->getUserFilePath());

    return result;
}

bool FileStorageManager::updateSymbolDirectoryOfFile(const QString &pathToSymbolFile, const QString &newSymbolDir)
{
    PtrTo_RowFileRecord rowRecord = QueryFileRecord(this->db).selectRowBySymbolFilePath(pathToSymbolFile);

    if(!rowRecord->isExistInDB())
        return false;

    if(rowRecord->getSymbolDirectory() == newSymbolDir)
        return true;

    bool result = rowRecord->setSymbolDirectory(newSymbolDir);

    return result;
}

bool FileStorageManager::updateUserDirectoryOfFile(const QString &pathToSymbolFile, const QString &newUserDir)
{
    PtrTo_RowFileRecord rowRecord = QueryFileRecord(this->db).selectRowBySymbolFilePath(pathToSymbolFile);

    if(!rowRecord->isExistInDB())
        return false;

    if(rowRecord->getUserDirectory() == newUserDir)
        return true;

    auto oldUserFilePath = rowRecord->getUserFilePath();
    bool result = rowRecord->setUserDirectory(newUserDir);

//    if(result == true)
//        emit signalMonitoredFilePathChangedByBackend(oldUserFilePath, rowRecord->getUserFilePath());

    return result;
}

bool FileStorageManager::updateFrozenStatusOfFile(const QString &pathToSymbolFile, bool isFrozen)
{
    PtrTo_RowFileRecord rowRecord = QueryFileRecord(this->db).selectRowBySymbolFilePath(pathToSymbolFile);

    if(!rowRecord->isExistInDB())
        return false;

    if(rowRecord->getIsFrozen() == isFrozen)
        return true;

    bool result = rowRecord->setIsFrozen(isFrozen);

    //if(result == true && isFrozen == true)
        //emit signalMonitoredFileRemovedByBackend(rowRecord->getUserFilePath());

    return result;
}

bool FileStorageManager::updateVersionNumberOfFile(const QString &pathToSymbolFile, qlonglong oldNumber, qlonglong newNumber)
{
    PtrTo_RowFileRecord rowRecord = QueryFileRecord(this->db).selectRowBySymbolFilePath(pathToSymbolFile);

    if(!rowRecord->isExistInDB())
        return false;

    PtrTo_RowFileVersion rowVersion = rowRecord->getRowFileVersion(oldNumber);

    if(!rowVersion->isExistInDB())
        return false;

    if(rowVersion->getVersionNumber() == newNumber)
        return true;

    bool result = rowVersion->setVersionNumber(newNumber);

    return result;
}

bool FileStorageManager::updateContentOfFile(const QString &pathToSymbolFile, qlonglong versionNumber, const QString &pathToNewContent)
{
    QString internalFileName, fileHash;
    qlonglong fileSize;

    PtrTo_RowFileEvent rowUnRegisteredFileEvent = this->addUnRegisteredFile(pathToNewContent,
                                                                      &internalFileName,
                                                                      &fileHash,
                                                                      &fileSize);

    if(!rowUnRegisteredFileEvent->isExistInDB())
        return false;

    bool isTransactionSuccessful = this->transactionUpdateRowFileVersion(pathToSymbolFile,
                                                                         versionNumber,
                                                                         internalFileName,
                                                                         fileHash,
                                                                         fileSize,
                                                                         rowUnRegisteredFileEvent);

    if(!isTransactionSuccessful)
        return false;

    bool result = this->deleteUnRegisteredFile(rowUnRegisteredFileEvent);

    return result;
}

bool FileStorageManager::markFileAsFavorite(const QString &pathToSymbolFile, bool status)
{
    auto rowFileRecord = QueryFileRecord(this->db).selectRowBySymbolFilePath(pathToSymbolFile);

    if(!rowFileRecord->isExistInDB())
        return false;

    bool result = rowFileRecord->setIsFavorite(status);

    return result;
}

void FileStorageManager::incrementSaveGroupNumber()
{
    this->currentSaveGroupNumber += 1;
}

bool FileStorageManager::isFileExistByUserFilePath(const QString &userFilePath) const
{
    bool queryResult = QueryFileRecord(this->db).isRowExistByUserFilePath(userFilePath);

    return queryResult;
}

bool FileStorageManager::isFolderExistByUserFolderPath(const QString &userFolderPath) const
{
    bool result = false;
    auto queryResult = QueryFileRecord(this->db).selectUserFolderPathListFromAllFiles();

    // TODO: Replace for loop with db query.
    for(const QString &currentPath : queryResult)
    {
        if(userFolderPath == currentPath)
        {
            result = true;
            break;
        }
    }

    return result;
}

QStringList FileStorageManager::getMonitoredFilePathList() const
{
    auto queryResult = QueryFileRecord(this->db).selectUserFilePathListFromActiveFiles();

    return queryResult;
}

QStringList FileStorageManager::getMonitoredFolderPathList() const
{
    auto queryResult = QueryFileRecord(this->db).selectUserFolderPathListFromActiveFiles();

    return queryResult;
}

qlonglong FileStorageManager::getCurrentSaveGroupNumber() const
{
    return this->currentSaveGroupNumber;
}

QList<qlonglong> FileStorageManager::getAvailableSaveGroupNumbers() const
{
    auto result = QuerySaveGroupItem(this->db).selectSaveGroupIDList();

    return result;
}

QList<SaveGroupItemMetaData> FileStorageManager::getSaveGroupItems(qlonglong saveGropuNumber) const
{
    QList<SaveGroupItemMetaData> result;

    auto queryResult = QuerySaveGroupItem(this->db).selectRowsInSaveGroup(saveGropuNumber);

    for(const auto rowSaveGroupItem : queryResult)
    {
        result.append(rowSaveGroupItem);
    }

    return result;
}

FolderRequestResult FileStorageManager::getFolderMetaData(const QString &directory) const
{
    QString dir = directory;
    if(!directory.endsWith(CONST_SYMBOL_DIRECTORY_SEPARATOR))
        dir.append(CONST_SYMBOL_DIRECTORY_SEPARATOR);

    FolderRequestResult result;

    auto rowFolder = QueryFolderRecord(this->db).selectRowByDirectory(dir);

    if(rowFolder->isExistInDB())
        result = FolderRequestResult(rowFolder);

    return result;
}

QList<FolderRequestResult> FileStorageManager::getFavoriteFolderMetaDataList() const
{
    QList<FolderRequestResult> result;

    auto queryResult = QueryFolderRecord(this->db).selectFavoriteFolderList();

    for(auto currentRow : queryResult)
    {
        FolderRequestResult item(currentRow);
        result.append(item);
    }

    return result;
}

FileRequestResult FileStorageManager::getFileMetaData(const QString &pathToSymbolFile) const
{
    FileRequestResult result;

    auto rowRecord = QueryFileRecord(this->db).selectRowBySymbolFilePath(pathToSymbolFile);

    if(rowRecord->isExistInDB())
        result = FileRequestResult(rowRecord);

    return result;
}

QList<FileRequestResult> FileStorageManager::getFavoriteFileMetaDataList() const
{
    QList<FileRequestResult> result;

    auto queryResult = QueryFileRecord(this->db).selectFavoriteFileList();

    for(auto currentRow : queryResult)
    {
        FileRequestResult item(currentRow);
        result.append(item);
    }

    return result;
}

FileVersionMetaData FileStorageManager::getFileVersionMetaData(const QString &pathToSymbolFile, qlonglong versionNumber) const
{
    FileVersionMetaData result;

    auto rowRecord = QueryFileRecord(this->db).selectRowBySymbolFilePath(pathToSymbolFile);

    if(!rowRecord->isExistInDB())
        return result;

    auto rowVersion = rowRecord->getRowFileVersion(versionNumber);

    if(!rowVersion->isExistInDB())
        return result;

    result = FileVersionMetaData(rowVersion);

    return result;
}

SaveGroupItemMetaData FileStorageManager::getSaveGroupItemMetaData(const QString &pathToSymbolFile, qlonglong versionNumber) const
{
    SaveGroupItemMetaData result;

    auto rowFileRecord = QueryFileRecord(this->db).selectRowBySymbolFilePath(pathToSymbolFile);

    if(rowFileRecord->isExistInDB())
    {
        auto rowFileVersion = rowFileRecord->getRowFileVersion(versionNumber);

        if(rowFileVersion->isExistInDB())
        {
            auto queryResult = rowFileVersion->getSaveGroupItem();

            result = SaveGroupItemMetaData(queryResult);
        }
    }

    return result;
}

const QString &FileStorageManager::rootFolderPath()
{
    return CONST_SYMBOL_DIRECTORY_SEPARATOR;
}

bool FileStorageManager::addNewFolder(const QString &directory)
{
    QString dir = directory;
    if(!directory.endsWith(CONST_SYMBOL_DIRECTORY_SEPARATOR))
        dir.append(CONST_SYMBOL_DIRECTORY_SEPARATOR);

    if(!this->isFolderExist(dir))
    {
        dir.truncate(dir.lastIndexOf(CONST_SYMBOL_DIRECTORY_SEPARATOR));

        QStringList tokenList = dir.split(CONST_SYMBOL_DIRECTORY_SEPARATOR);
        PtrTo_RowFolderRecord currentFolder;

        auto *rowInserter = new RowInserter(this->db);
        ScopedPtrTo_RowFolderRecordInserter folderInserter(rowInserter);

        for(auto const &token : tokenList)
        {
            if(token.isEmpty())
            {
                currentFolder = folderInserter->insertRootFolder(CONST_SYMBOL_DIRECTORY_SEPARATOR);

                if(!currentFolder->isExistInDB())
                    return false;
            }
            else
            {
                QString suffix = token + CONST_SYMBOL_DIRECTORY_SEPARATOR;
                bool isChildAdded = currentFolder->addChildFolder(suffix);

                if(!isChildAdded)
                    return false;
                else
                    currentFolder = currentFolder->getChildFolderBySuffix(suffix);
            }
        }

        return true;
    }

    return true;
}

bool FileStorageManager::markFolderAsFavorite(const QString &directory, bool status)
{
    auto rowFolder = QueryFolderRecord(this->db).selectRowByDirectory(directory);

    if(!rowFolder->isExistInDB())
        return false;

    bool result = rowFolder->setIsFavorite(status);

    return result;
}

bool FileStorageManager::isFolderExist(const QString &directory) const
{
    QString dir = directory;

    if(!directory.endsWith(CONST_SYMBOL_DIRECTORY_SEPARATOR))
        dir.append(CONST_SYMBOL_DIRECTORY_SEPARATOR);

    PtrTo_RowFolderRecord rowFolder = QueryFolderRecord(this->db).selectRowByDirectory(dir);

    if(rowFolder->isExistInDB())
        return true;

    return false;
}

bool FileStorageManager::deleteFolder(const QString &directory)
{
    QString dir = directory;

    if(!directory.endsWith(CONST_SYMBOL_DIRECTORY_SEPARATOR))
        dir.append(CONST_SYMBOL_DIRECTORY_SEPARATOR);

    PtrTo_RowFolderRecord rowFolder = QueryFolderRecord(this->db).selectRowByDirectory(dir);

    if(!rowFolder->isExistInDB())
        return false;

    QList<PtrTo_RowFolderRecord> parentFolders;
    parentFolders.append(rowFolder);

    QList<PtrTo_RowFileRecord> targetFiles;
    QStringList userFilePathList;

    while(!parentFolders.isEmpty())
    {
        parentFolders << parentFolders.first()->getAllChildRowFolderRecords();
        targetFiles << parentFolders.first()->getAllChildRowFileRecords();
        parentFolders.removeFirst();
    }

    for(const auto currentRowFileRecord : targetFiles)
    {
        userFilePathList << currentRowFileRecord->getUserFilePath();
    }

    QList<PtrTo_RowFileEvent> events = this->transactionDeleteRowFileRecord(targetFiles);

    if(events.isEmpty())
        return false;

//    for(const auto currentUserFilePath : userFilePathList)
//    {
//        emit signalMonitoredFileRemovedByBackend(currentUserFilePath);
//    }

    for(const auto &currentEvent : events)
    {
        bool isFileDeleted = this->deleteUnRegisteredFile(currentEvent);

        if(!isFileDeleted)
            return false;
    }

    RowDeleter folderDeleter(rowFolder);

    bool result = folderDeleter.deleteRow();

    return result;
}

bool FileStorageManager::renameFolder(const QString &directory, const QString &newSuffix)
{
    QString dir = directory;

    if(!directory.endsWith(CONST_SYMBOL_DIRECTORY_SEPARATOR))
        dir.append(CONST_SYMBOL_DIRECTORY_SEPARATOR);

    PtrTo_RowFolderRecord rowFolder = QueryFolderRecord(this->db).selectRowByDirectory(dir);

    if(!rowFolder->isExistInDB())
        return false;

    QString suffix = newSuffix;

    if(!newSuffix.endsWith(CONST_SYMBOL_DIRECTORY_SEPARATOR))
        suffix = suffix.append(CONST_SYMBOL_DIRECTORY_SEPARATOR);

    if(rowFolder->getSuffixDirectory() == suffix)
        return true;

    bool result = rowFolder->setSuffixDirectory(suffix);

    return result;
}

const QString &FileStorageManager::getBackupDirectory() const
{
    return backupDirectory;
}

FileStorageManager::~FileStorageManager()
{
    this->db.close();
}

PtrTo_RowFolderRecord FileStorageManager::getRootFolderSymbol() const
{
    QueryFolderRecord queries(this->db);

    PtrTo_RowFolderRecord result = queries.selectRowByDirectory(CONST_SYMBOL_DIRECTORY_SEPARATOR);

    return result;
}

PtrTo_RowFileEvent FileStorageManager::addUnRegisteredFile(const QString &pathToSourceFile,
                                                     QString *resultInternalFileName,
                                                     QString *resultFileHash,
                                                     qlonglong *resultSize)
{
    *resultInternalFileName = INVALID_FIELD_VALUE_QSTRING;
    *resultFileHash = INVALID_FIELD_VALUE_QSTRING;
    *resultSize = INVALID_FIELD_VALUE_QLONGLONG;
    PtrTo_RowFileEvent resultEmptyEvent(new RowFileEvent());

    QFile sourceFile(pathToSourceFile);
    QString internalFileName = this->generateInternalFileName();
    QString internalFilePath = this->getBackupDirectory() + internalFileName;

    bool isFileExist = sourceFile.exists();

    if(!isFileExist)
        return resultEmptyEvent;

    bool isFileOpened = sourceFile.open(QFile::OpenModeFlag::ReadOnly);

    if(!isFileOpened)
        return resultEmptyEvent;

    QString fileHash = this->getHashOf(sourceFile.readAll());
    sourceFile.close();

    auto *rowInserter = new RowInserter(this->db);
    ScopedPtrTo_RowFileEventInserter eventInserter(rowInserter);
    PtrTo_RowFileEvent rowUnRegisteredFileEvent = eventInserter->insertUnRegisteredFileEvent(internalFileName);

    bool isFileCopied = sourceFile.copy(internalFilePath);

    if(!isFileCopied)
        return resultEmptyEvent;

    if(rowUnRegisteredFileEvent->isExistInDB())
    {
        *resultInternalFileName = internalFileName;
        *resultFileHash = fileHash;
        *resultSize = sourceFile.size();

        return rowUnRegisteredFileEvent;
    }
    else
        return resultEmptyEvent;
}

bool FileStorageManager::deleteUnRegisteredFile(PtrTo_RowFileEvent event)
{
    if(event->isUnRegisteredFileEvent())
    {
        QFile targetFile(this->getBackupDirectory() + event->getData());

        if(targetFile.exists())
        {
            bool isFileDeleted = targetFile.remove();

            if(!isFileDeleted)
                return false;
        }

        RowDeleter deleteEvent(event);
        bool result = deleteEvent.deleteRow();

        return result;
    }

    return false;
}

bool FileStorageManager::transactionInsertRowFileRecord(const QString &fileName,
                                                        const QString &fileExtension,
                                                        const QString &symbolDirectory,
                                                        const QString &userDirectory,
                                                        bool isFrozen,
                                                        bool isAutoSyncEnabled,
                                                        qlonglong fileSize,
                                                        const QString &fileHash,
                                                        const QString &description,
                                                        const QString &internalFileName,
                                                        PtrTo_RowFileEvent unRegisteredFileEvent)
{
    if(this->db.transaction())
    {
        auto *rowInserter = new RowInserter(this->db);
        ScopedPtrTo_RowFileRecordInserter fileRecordInserter(rowInserter);
        PtrTo_RowFileRecord rowRecord;

        if(isFrozen == false)
        {
            rowRecord = fileRecordInserter->insertActiveFile(fileName, fileExtension, symbolDirectory, userDirectory, isAutoSyncEnabled);
        }
        else
        {
            if(userDirectory.isEmpty())
                rowRecord = fileRecordInserter->insertFrozenFile(fileName, fileExtension, symbolDirectory, isAutoSyncEnabled);
            else
                rowRecord = fileRecordInserter->insertFrozenFile(fileName, fileExtension, symbolDirectory, isAutoSyncEnabled, userDirectory);
        }

        if(rowRecord->isExistInDB())
        {
            auto rowVersion = rowRecord->insertVersion(1,
                                                       internalFileName,
                                                       fileSize,
                                                       fileHash,
                                                       description);

            if(rowVersion->isExistInDB())
            {
                auto rowSaveGroupItem = rowVersion->includeInSaveGroup(this->getCurrentSaveGroupNumber());

                if(rowSaveGroupItem->isExistInDB())
                {
                    RowDeleter deleteEvent(unRegisteredFileEvent);
                    bool isUnRegisterEventDeleted = deleteEvent.deleteRow();

                    if(isUnRegisterEventDeleted)
                    {
                        if(this->db.commit())
                        {
                            return true;
                        }
                    }
                }
            }
        }

        this->db.rollback();
    }

    return false;
}

bool FileStorageManager::transactionInsertRowFileVersion(const QString &pathToSymbolFile,
                                                         qlonglong versionNumber,
                                                         const QString &internalFileName,
                                                         qlonglong fileSize,
                                                         const QString &fileHash,
                                                         const QString &description,
                                                         PtrTo_RowFileEvent unRegisteredFileEvent)
{
    PtrTo_RowFileRecord parentRecord = QueryFileRecord(this->db).selectRowBySymbolFilePath(pathToSymbolFile);

    if(!parentRecord->isExistInDB())
        return false;

    if(this->db.transaction())
    {
        if(parentRecord->isExistInDB())
        {
            auto rowVersion = parentRecord->insertVersion(versionNumber,
                                                         internalFileName,
                                                         fileSize,
                                                         fileHash,
                                                         description);

            if(rowVersion->isExistInDB())
            {
                auto rowSaveGroupItem = rowVersion->includeInSaveGroup(this->getCurrentSaveGroupNumber());

                if(rowSaveGroupItem->isExistInDB())
                {
                    RowDeleter rowDeleter(unRegisteredFileEvent);
                    bool isEventDeleted = rowDeleter.deleteRow();

                    if(isEventDeleted)
                    {
                        if(this->db.commit())
                            return true;
                    }
                }
            }

            this->db.rollback();
        }
    }

    return false;
}

bool FileStorageManager::transactionUpdateRowFileVersion(const QString &pathToSymbolFile,
                                                         qlonglong versionNumber,
                                                         const QString &internalFileName,
                                                         const QString &newHash,
                                                         qlonglong fileSize,
                                                         PtrTo_RowFileEvent rowUnRegisteredFileEvent)
{
    PtrTo_RowFileRecord rowRecord = QueryFileRecord(this->db).selectRowBySymbolFilePath(pathToSymbolFile);

    if(!rowRecord->isExistInDB())
        return false;

    PtrTo_RowFileVersion rowVerison = rowRecord->getRowFileVersion(versionNumber);

    if(!rowVerison->isExistInDB())
        return false;

    if(this->db.transaction())
    {
        bool isOldFileNameSaved = rowUnRegisteredFileEvent->setData(rowVerison->getInternalFileName());

        if(!isOldFileNameSaved)
        {
            this->db.rollback();
            return false;
        }

        bool isNameUpdated = rowVerison->setInternalFileName(internalFileName);
        bool isHashUpdated = rowVerison->setHash(newHash);
        bool isSizeUpdated = rowVerison->setSize(fileSize);
        bool isTimestampUpdated = rowVerison->setTimestamp(QDateTime::currentDateTime());

        if(!isNameUpdated || !isHashUpdated || !isSizeUpdated || !isTimestampUpdated)
        {
            this->db.rollback();
            return false;
        }

        if(!this->db.commit())
        {
            this->db.rollback();
            return false;
        }
    }

    return true;
}

QList<PtrTo_RowFileEvent> FileStorageManager::transactionDeleteRowFileRecord(QList<PtrTo_RowFileRecord> targetRecords)
{
    QList<PtrTo_RowFileEvent> result;
    QList<PtrTo_RowFileEvent> emptyList;

    if(targetRecords.isEmpty())
        return emptyList;

    if(this->db.transaction())
    {
        for(auto &currentRecord : targetRecords)
        {
            for(auto &currentVersion : currentRecord->getAllRowFileVersions())
            {
                PtrTo_RowFileEvent unRegisterEvent = currentVersion->markAsUnRegistered();

                if(!unRegisterEvent->isExistInDB())
                {
                    this->db.rollback();
                    return emptyList;
                }
                else
                    result.append(unRegisterEvent);
            }

            RowDeleter deleteRecord(currentRecord);
            bool isDeleted = deleteRecord.deleteRow();

            if(!isDeleted)
            {
                this->db.rollback();
                return emptyList;
            }
        }

        if(this->db.commit())
            return result;
        else
        {
            this->db.rollback();
            return emptyList;
        }

    }

    return emptyList;
}

QList<PtrTo_RowFileEvent> FileStorageManager::transactionDeleteRowFileVersion(QList<PtrTo_RowFileVersion> targetVersions)
{
    QList<PtrTo_RowFileEvent> emptyList, result;
    PtrTo_RowFileRecord rowRecord = targetVersions.first()->getParentRowFileRecord();

    if(this->db.transaction())
    {
        for(auto currentRowFileVersion : targetVersions)
        {
            PtrTo_RowFileEvent markedEvent = currentRowFileVersion->markAsUnRegistered();

            if(!markedEvent->isExistInDB())
            {
                this->db.rollback();
                return emptyList;
            }

            result.append(markedEvent);
        }

        qlonglong versionCount = rowRecord->getRowFileVersionCount();
        bool isRowDeleted = false;

        if(versionCount == targetVersions.size())
        {
            RowDeleter recordDeleter(rowRecord);
            isRowDeleted = recordDeleter.deleteRow();
        }
        else
        {
            for(auto &currentRowFileVersion : targetVersions)
            {
                RowDeleter versionDeleter(currentRowFileVersion);
                isRowDeleted = versionDeleter.deleteRow();

                if(isRowDeleted == false)
                    break;
            }
        }

        if(!isRowDeleted)
        {
            this->db.rollback();
            return emptyList;
        }

        if(this->db.commit())
            return result;
        else
        {
            this->db.rollback();
            return emptyList;
        }
    }

    return emptyList;
}

QString FileStorageManager::getHashOf(const QByteArray &input) const
{
    QCryptographicHash hasher(QCryptographicHash::Algorithm::Sha3_256);
    hasher.addData(input);

    return QString(hasher.result().toHex());
}

QString FileStorageManager::generateInternalFileName() const
{
    QString uniquePart = QUuid::createUuid().toString(QUuid::StringFormat::Id128);
    QString fileName = uniquePart + INTERNAL_FILE_NAME_EXTENSION;

    do
    {
        QFileInfo info(this->getBackupDirectory() + fileName);
        bool isFileNameUnique = info.exists();

        if(isFileNameUnique) // If collision happened.
        {
            uniquePart += QUuid::createUuid().toString(QUuid::StringFormat::Id128);
            fileName = uniquePart + INTERNAL_FILE_NAME_EXTENSION;
        }
        else
            break;
    }
    while (true);

    return fileName;
}

void FileStorageManager::extractSqliteDBIfNotExist()
{
    QDir().mkdir(this->getBackupDirectory());
    QString dbFilePath = QDir::toNativeSeparators(this->getBackupDirectory() + DB_FILE_NAME);
    QFile dbFile(dbFilePath);

    if(!dbFile.exists())
    {
        QFile db_file_res(":/resources/sql/" + DB_FILE_NAME);
        db_file_res.copy(dbFilePath);
        dbFile.setPermissions(QFileDevice::Permission::ReadOwner | QFileDevice::Permission::WriteOwner);
    }
}
