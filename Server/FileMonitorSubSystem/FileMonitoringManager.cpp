#include "FileMonitoringManager.h"

FileMonitoringManager::FileMonitoringManager(FileStorageManager *fsm,
                                             FileSystemEventStore *fses,
                                             QObject *parent)
    : QObject{parent}
{
    this->fsm = fsm;
    this->fses = fses;

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

FileMonitoringManager::~FileMonitoringManager()
{
    QHashIterator<QString, efsw::WatchID> hashIter(watchedFolderMap);

    hashIter.next();

    while(hashIter.hasNext())
    {
        fileWatcher.removeWatch(hashIter.value());
        hashIter.next();
    }

    delete fsm;
    delete fses;
    fsm = nullptr;
    fses = nullptr;
}

void FileMonitoringManager::addFolder(const QString &userFolderPath)
{
    efsw::WatchID watchId = fileWatcher.addWatch(userFolderPath.toStdString(), &fileSystemEventListener, false);

    if(watchId >= efsw::Errors::NoError)
        watchedFolderMap.insert(userFolderPath, watchId);
}

void FileMonitoringManager::slotOnAddEventDetected(const QString &fileOrFolderName, const QString &dir)
{
    fses->addFolder(fileOrFolderName + dir, FileSystemEventStore::Status::NewAdded);
}

void FileMonitoringManager::slotOnDeleteEventDetected(const QString &fileOrFolderName, const QString &dir)
{
    fses->addFolder(fileOrFolderName + dir, FileSystemEventStore::Status::Deleted);
}

void FileMonitoringManager::slotOnModificationEventDetected(const QString &fileName, const QString &dir)
{
    fses->addFolder(fileName + dir, FileSystemEventStore::Status::Updated);
}

void FileMonitoringManager::slotOnMoveEventDetected(const QString &fileOrFolderName, const QString &oldFileOrFolderName, const QString &dir)
{
    fses->addFolder(fileOrFolderName + dir, FileSystemEventStore::Status::Renamed);
}
