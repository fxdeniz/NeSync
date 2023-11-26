#include "FileMonitoringManager.h"
#include "Utility/JsonDtoFormat.h"
#include "FileStorageSubSystem/FileStorageManager.h"

#include <QDir>
#include <QFileInfo>

FileMonitoringManager::FileMonitoringManager(FileSystemEventDb *fsEventDb, QObject *parent)
    : QObject{parent}
{
    this->eventDb = fsEventDb;

    QObject::connect(&fileSystemEventListener, &FileSystemEventListener::signalAddEventDetected,
                     this, &FileMonitoringManager::slotOnAddEventDetected);

    QObject::connect(&fileSystemEventListener, &FileSystemEventListener::signalDeleteEventDetected,
                     this, &FileMonitoringManager::slotOnDeleteEventDetected);

    QObject::connect(&fileSystemEventListener, &FileSystemEventListener::signalModificationEventDetected,
                     this, &FileMonitoringManager::slotOnModificationEventDetected);

    QObject::connect(&fileSystemEventListener, &FileSystemEventListener::signalMoveEventDetected,
                     this, &FileMonitoringManager::slotOnMoveEventDetected);

    fileWatcher.watch();
}

bool FileMonitoringManager::addFolder(const QString &userFolderPath)
{
    if(eventDb->isMonitoredFolderExist(userFolderPath))
        return false;

    efsw::WatchID watchId = fileWatcher.addWatch(userFolderPath.toStdString(), &fileSystemEventListener, false);

    if(watchId >= efsw::Errors::NoError)
    {
        eventDb->addMonitoredFolder(userFolderPath, watchId);
        return true;
    }

    return false;
}

bool FileMonitoringManager::addFile(const QString &userFolderPath, const QString &fileName)
{
    if(!eventDb->isMonitoredFolderExist(userFolderPath))
        return false;

    if(eventDb->isMonitoredFileExist(userFolderPath, fileName))
        return false;

    QFileInfo info(userFolderPath + fileName);

    bool isExist = info.exists();
    bool isFile = info.isFile();

    if(!isExist || !isFile)
        return false;

    eventDb->addMonitoredFile(userFolderPath, fileName);

    return true;
}

void FileMonitoringManager::slotOnAddEventDetected(const QString &fileOrFolderName, const QString &dir)
{
    QString currentPath = QDir::toNativeSeparators(dir + fileOrFolderName);

    qDebug() << "addEvent = " << currentPath;
    qDebug() << "";

    QFileInfo info(currentPath);

    if(info.isDir())
        handleFolderAddEvent(dir, fileOrFolderName);
    else if(info.isFile() && !info.isHidden()) // Only accept real files
        handleFileAddEvent(dir, fileOrFolderName);
}

void FileMonitoringManager::slotOnDeleteEventDetected(const QString &fileOrFolderName, const QString &dir)
{
    qDebug() << "deleteEvent = " << dir << fileOrFolderName;
    qDebug() << "";

    QString _dir = QDir::toNativeSeparators(dir);

    handleFolderDeleteEvent(_dir, fileOrFolderName);
    handleFileDeleteEvent(_dir, fileOrFolderName);
}

void FileMonitoringManager::slotOnModificationEventDetected(const QString &fileName, const QString &dir)
{
    qDebug() << "updateEvent = " << dir << fileName;
    qDebug() << "";

    QString _dir = QDir::toNativeSeparators(dir);
    handleFileModificationEvent(_dir, fileName);
}

void FileMonitoringManager::slotOnMoveEventDetected(const QString &fileOrFolderName, const QString &oldFileOrFolderName, const QString &dir)
{
    qDebug() << "renameEvent (old) -> (new) = " << oldFileOrFolderName << fileOrFolderName << dir;
    qDebug() << "";

    QString _dir = QDir::toNativeSeparators(dir);
    if(!_dir.endsWith(QDir::separator()))
        _dir.append(QDir::separator());

    QString currentNewPath = _dir + fileOrFolderName;
    QFileInfo info(currentNewPath);

    if(info.isDir())
        handleFolderMoveEvent(_dir, oldFileOrFolderName, fileOrFolderName);
    else if(info.isFile() && !info.isHidden())
        handleFileMoveEvent(_dir, oldFileOrFolderName, fileOrFolderName);
}

void FileMonitoringManager::handleFolderAddEvent(const QString &parentDirPath, const QString &folderName)
{
    QString currentPath = parentDirPath + folderName;

    if(!currentPath.endsWith(QDir::separator()))
        currentPath.append(QDir::separator());

    efsw::WatchID watchID = fileWatcher.addWatch(currentPath.toStdString(), &fileSystemEventListener, false);
    // TODO: Add watchID error checking

    auto fsm = FileStorageManager::instance();
    QJsonObject folderJson = fsm->getFolderJsonByUserPath(currentPath);
    bool isFolderPersists = folderJson[JsonKeys::IsExist].toBool();
    bool isFolderFrozen = folderJson[JsonKeys::Folder::IsFrozen].toBool();

    if(!isFolderPersists)
        eventDb->addNewAddedFolder(currentPath, watchID);
    else if(isFolderPersists && !isFolderFrozen)
        eventDb->setStatusOfMonitoredFolder(currentPath, FileSystemEventDb::ItemStatus::Updated);
}

void FileMonitoringManager::handleFileAddEvent(const QString &parentDirPath, const QString &fileName)
{
    QString _parentDirPath = parentDirPath;

    if(!_parentDirPath.endsWith(QDir::separator()))
        _parentDirPath.append(QDir::separator());

    QString currentPath = _parentDirPath + fileName;

    auto fsm = FileStorageManager::instance();
    QJsonObject fileJson = fsm->getFileJsonByUserPath(currentPath);
    bool isFilePersists = fileJson[JsonKeys::IsExist].toBool();
    bool isFileFrozen = fileJson[JsonKeys::File::IsFrozen].toBool();

    if(!eventDb->isMonitoredFileExist(_parentDirPath, fileName))
        eventDb->addNewAddedFile(_parentDirPath, fileName);
    else if(isFilePersists & !isFileFrozen) // TODO: also add hash comparison here
        eventDb->setStatusOfMonitoredFile(_parentDirPath, fileName, FileSystemEventDb::ItemStatus::Updated);
}

void FileMonitoringManager::handleFolderDeleteEvent(const QString &parentDirPath, const QString &folderName)
{
    QString _parentDirPath = parentDirPath;

    if(!_parentDirPath.endsWith(QDir::separator()))
        _parentDirPath.append(QDir::separator());

    QString _folderName = folderName;

    if(!_folderName.endsWith(QDir::separator()))
        _folderName.append(QDir::separator());

    QString currentPath = _parentDirPath + _folderName;

    if(eventDb->getNewAddedFolderMap().contains(currentPath))
        eventDb->removeNewAddedFolder(currentPath);

    auto fsm = FileStorageManager::instance();

    if(!eventDb->getOriginalFolderNameByNewName(currentPath).isEmpty())
        currentPath = eventDb->getOriginalFolderNameByNewName(currentPath);

    QJsonObject folderJson = fsm->getFolderJsonByUserPath(currentPath);

    bool isFolderPersists = folderJson[JsonKeys::IsExist].toBool();
    bool isFolderFrozen = folderJson[JsonKeys::Folder::IsFrozen].toBool();

    if(isFolderPersists && !isFolderFrozen)
    {
        eventDb->setStatusOfMonitoredFolder(currentPath, FileSystemEventDb::ItemStatus::Deleted);
        eventDb->removeFolderRenamingEntry(_parentDirPath + _folderName);
    }
}

void FileMonitoringManager::handleFileDeleteEvent(const QString &parentDirPath, const QString &fileName)
{
    QString _parentDirPath = parentDirPath;

    if(!_parentDirPath.endsWith(QDir::separator()))
        _parentDirPath.append(QDir::separator());

    QString currentPath = _parentDirPath + fileName;

    if(eventDb->getNewAddedFileSet(_parentDirPath).contains(fileName))
        eventDb->removeNewAddedFile(_parentDirPath, fileName);

    auto fsm = FileStorageManager::instance();

    if(!eventDb->getOriginalFileNameByNewName(_parentDirPath, fileName).isEmpty())
        currentPath = eventDb->getOriginalFileNameByNewName(_parentDirPath, fileName);

    QJsonObject fileJson = fsm->getFileJsonByUserPath(currentPath);

    bool isFilePersists = fileJson[JsonKeys::IsExist].toBool();
    bool isFileFrozen = fileJson[JsonKeys::File::IsFrozen].toBool();

    if(isFilePersists && !isFileFrozen)
    {
        eventDb->setStatusOfMonitoredFile(_parentDirPath, fileJson[JsonKeys::File::FileName].toString(), FileSystemEventDb::ItemStatus::Deleted);
        eventDb->removeFileRenamingEntry(_parentDirPath, fileName);
    }
}

void FileMonitoringManager::handleFolderMoveEvent(const QString &parentDirPath, const QString &oldFolderName, const QString &newFolderName)
{
    QString _parentDirPath = parentDirPath;

    if(!_parentDirPath.endsWith(QDir::separator()))
        _parentDirPath.append(QDir::separator());

    QString currentOldPath = _parentDirPath + oldFolderName;
    QString currentNewPath = _parentDirPath + newFolderName;

    if(!currentOldPath.endsWith(QDir::separator()))
        currentOldPath.append(QDir::separator());

    if(!currentNewPath.endsWith(QDir::separator()))
        currentNewPath.append(QDir::separator());

    auto fsm = FileStorageManager::instance();

    QJsonObject oldFolderJson = fsm->getFolderJsonByUserPath(currentOldPath);

    bool isOldFolderPersists = oldFolderJson[JsonKeys::IsExist].toBool();
    bool isOldFolderFrozen = oldFolderJson[JsonKeys::Folder::IsFrozen].toBool();

    if(isOldFolderPersists && !isOldFolderFrozen)
        eventDb->setStatusOfMonitoredFolder(currentOldPath, FileSystemEventDb::ItemStatus::Renamed);

    if(!eventDb->getNewAddedFolderMap().contains(currentOldPath))
        eventDb->addFolderRenamingEntry(currentOldPath, currentNewPath);
    else
    {
        efsw::WatchID watchID = eventDb->getNewAddedFolderMap().value(currentOldPath);
        eventDb->removeNewAddedFolder(currentOldPath);
        eventDb->addNewAddedFolder(currentNewPath, watchID);
    }
}

void FileMonitoringManager::handleFileMoveEvent(const QString &parentDirPath, const QString &oldFileName, const QString &newFileName)
{
    QString _parentDirPath = parentDirPath;

    if(!_parentDirPath.endsWith(QDir::separator()))
        _parentDirPath.append(QDir::separator());

    QString currentOldPath = _parentDirPath + oldFileName;
    QString currentNewPath = _parentDirPath + newFileName;

    auto fsm = FileStorageManager::instance();

    QJsonObject newFileJson = fsm->getFileJsonByUserPath(currentNewPath);

    bool isNewFilePersists = newFileJson[JsonKeys::IsExist].toBool();
    bool isNewFileFrozen = newFileJson[JsonKeys::File::IsFrozen].toBool();

    if(isNewFilePersists && !isNewFileFrozen) // When moved file overwritten to persistent file
        eventDb->setStatusOfMonitoredFile(_parentDirPath, newFileName, FileSystemEventDb::ItemStatus::Updated);
    else
    {
        QJsonObject oldFileJson = fsm->getFileJsonByUserPath(currentOldPath);

        bool isOldFilePersists = oldFileJson[JsonKeys::IsExist].toBool();
        bool isOldFileFrozen = oldFileJson[JsonKeys::File::IsFrozen].toBool();

        if(isOldFilePersists && !isOldFileFrozen)
            eventDb->setStatusOfMonitoredFile(_parentDirPath, oldFileName, FileSystemEventDb::ItemStatus::Renamed);

        if(!eventDb->getNewAddedFileSet(_parentDirPath).contains(oldFileName))
            eventDb->addFileRenamingEntry(_parentDirPath, oldFileName, newFileName);
        else
        {
            eventDb->removeNewAddedFile(_parentDirPath, oldFileName);
            eventDb->addNewAddedFile(_parentDirPath, newFileName);
        }
    }
}

void FileMonitoringManager::handleFileModificationEvent(const QString &parentDirPath, const QString &fileName)
{
    QString _parentDirPath = parentDirPath;

    if(!_parentDirPath.endsWith(QDir::separator()))
        _parentDirPath.append(QDir::separator());

    QString currentPath = _parentDirPath + fileName;

    auto fsm = FileStorageManager::instance();

    QJsonObject fileJson = fsm->getFileJsonByUserPath(currentPath);
    QString symbolFilePath = fileJson[JsonKeys::File::SymbolFilePath].toString();
    qlonglong maxVersionNumber = fileJson[JsonKeys::File::MaxVersionNumber].toInteger();

    QJsonObject versionJson = fsm->getFileVersionJson(symbolFilePath, maxVersionNumber);
    QString strLastModifiedTimestamp = versionJson[JsonKeys::FileVersion::LastModifiedTimestamp].toString();
    QDateTime lastModifiedTimestamp = QDateTime::fromString(strLastModifiedTimestamp, Qt::DateFormat::ISODateWithMs);
    QDateTime currentTimestamp = QFileInfo(currentPath).lastModified();

    bool isFilePersists = fileJson[JsonKeys::IsExist].toBool();
    bool isFileFrozen = fileJson[JsonKeys::File::IsFrozen].toBool();
    bool isFileTouched = (lastModifiedTimestamp != currentTimestamp);

    if(isFilePersists && !isFileFrozen && isFileTouched)
        eventDb->setStatusOfMonitoredFile(_parentDirPath, fileName, FileSystemEventDb::ItemStatus::Updated);
}
