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

void FileMonitoringManager::slotOnAddEventDetected(const QString &fileName, const QString &dir)
{
    QString currentPath = QDir::toNativeSeparators(dir + fileName);

    qDebug() << "addEvent = " << currentPath;
    qDebug() << "";

    QFileInfo info(currentPath);

    if(info.isDir())
    {
        auto fsm = FileStorageManager::instance();
        QJsonObject folderJson = fsm->getFolderJsonByUserPath(currentPath);
        bool isFolderPersists = folderJson[JsonKeys::IsExist].toBool();
        bool isFolderFrozen = folderJson[JsonKeys::Folder::IsFrozen].toBool();

        if(!eventDb->isMonitoredFolderExist(currentPath))
        {
            if(!isFolderPersists)
                eventDb->addNewAddedFolder(currentPath);
        }
        else
        {
            if(isFolderPersists && !isFolderFrozen)
                eventDb->setStatusOfMonitoredFolder(dir, FileSystemEventDb::ItemStatus::Updated);
        }
    }
    else if(info.isFile() && !info.isHidden()) // Only accept real files
    {
        auto fsm = FileStorageManager::instance();
        QJsonObject fileJson = fsm->getFileJsonByUserPath(currentPath);
        bool isFilePersists = fileJson[JsonKeys::IsExist].toBool();
        bool isFileFrozen = fileJson[JsonKeys::File::IsFrozen].toBool();

        if(!eventDb->isMonitoredFileExist(dir, fileName))
            eventDb->addNewAddedFile(dir, fileName);
        else if(isFilePersists & !isFileFrozen) // TODO: also add hash comparison here
            eventDb->setStatusOfMonitoredFile(dir, fileName, FileSystemEventDb::ItemStatus::Updated);
    }
}

void FileMonitoringManager::slotOnDeleteEventDetected(const QString &fileName, const QString &dir)
{
    qDebug() << "deleteEvent = " << dir << fileName;
    qDebug() << "";

    QString currentPath = QDir::toNativeSeparators(dir + fileName);

    if(eventDb->getNewAddedFolderSet().contains(currentPath))
        eventDb->removeNewAddedFolder(currentPath);

    else if(eventDb->getNewAddedFileSet(dir).contains(fileName))
        eventDb->removeNewAddedFile(dir, fileName);

    else if(eventDb->isMonitoredFolderExist(currentPath))
    {
        auto fsm = FileStorageManager::instance();
        QJsonObject folderJson = fsm->getFolderJsonByUserPath(currentPath);

        bool isFolderPersists = folderJson[JsonKeys::IsExist].toBool();
        bool isFolderFrozen = folderJson[JsonKeys::Folder::IsFrozen].toBool();

        if(isFolderPersists && !isFolderFrozen)
            eventDb->setStatusOfMonitoredFolder(currentPath, FileSystemEventDb::ItemStatus::Deleted);
    }
    else if(eventDb->isMonitoredFileExist(dir, fileName))
    {
        auto fsm = FileStorageManager::instance();
        QJsonObject fileJson = fsm->getFileJsonByUserPath(currentPath);

        bool isFilePersists = fileJson[JsonKeys::IsExist].toBool();
        bool isFileFrozen = fileJson[JsonKeys::File::IsFrozen].toBool();

        if(isFilePersists && !isFileFrozen)
            eventDb->setStatusOfMonitoredFile(dir, fileName, FileSystemEventDb::ItemStatus::Deleted);
    }
}

void FileMonitoringManager::slotOnModificationEventDetected(const QString &fileName, const QString &dir)
{
    qDebug() << "updateEvent = " << dir << fileName;
    qDebug() << "";

    bool isFileMonitored = eventDb->isMonitoredFileExist(dir, fileName);

    if(isFileMonitored)
    {
        QString currentPath = QDir::toNativeSeparators(dir + fileName);

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
            eventDb->setStatusOfMonitoredFile(dir, fileName, FileSystemEventDb::ItemStatus::Updated);
    }
}

void FileMonitoringManager::slotOnMoveEventDetected(const QString &fileName, const QString &oldFileName, const QString &dir)
{
    qDebug() << "renameEvent (old) -> (new) = " << oldFileName << fileName << dir;
    qDebug() << "";

    QString currentOldPath = QDir::toNativeSeparators(dir + oldFileName);
    QString currentNewPath = QDir::toNativeSeparators(dir + fileName);
    QFileInfo info(currentNewPath);

    if(info.isFile() && !info.isHidden())
    {
        auto fsm = FileStorageManager::instance();

        QJsonObject newFileJson = fsm->getFileJsonByUserPath(currentNewPath);

        bool isNewFilePersists = newFileJson[JsonKeys::IsExist].toBool();
        bool isNewFileFrozen = newFileJson[JsonKeys::File::IsFrozen].toBool();

        if(isNewFilePersists && !isNewFileFrozen) // When moved file overwritten to persistent file
            eventDb->setStatusOfMonitoredFile(dir, fileName, FileSystemEventDb::ItemStatus::Updated);
        else
        {
            eventDb->addRenamingEntry(currentOldPath, currentNewPath);

            if(eventDb->isMonitoredFileExist(dir, oldFileName))
                eventDb->setStatusOfMonitoredFile(dir, oldFileName, FileSystemEventDb::ItemStatus::Renamed);
        }
    }
}
