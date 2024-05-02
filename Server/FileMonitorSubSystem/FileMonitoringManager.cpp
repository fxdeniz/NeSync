#include "FileMonitoringManager.h"

FileMonitoringManager::FileMonitoringManager(FileStorageManager *fsm,
                                             FileSystemEventStore *fses,
                                             QObject *parent)
    : QObject{parent}
{
    this->fsm = fsm;
    this->fses = fses;

    fileSystemEventListener = new FileSystemEventListener(this);

    QObject::connect(fileSystemEventListener, &FileSystemEventListener::signalAddEventDetected,
                     this, &FileMonitoringManager::slotOnAddEventDetected);

    QObject::connect(fileSystemEventListener, &FileSystemEventListener::signalDeleteEventDetected,
                     this, &FileMonitoringManager::slotOnDeleteEventDetected);

    QObject::connect(fileSystemEventListener, &FileSystemEventListener::signalModificationEventDetected,
                     this, &FileMonitoringManager::slotOnModificationEventDetected);

    QObject::connect(fileSystemEventListener, &FileSystemEventListener::signalMoveEventDetected,
                     this, &FileMonitoringManager::slotOnMoveEventDetected);

    fileWatcher.watch();
}

FileMonitoringManager::~FileMonitoringManager()
{
    qDebug() << "Deleting fmm = " << this;

    QHashIterator<QString, efsw::WatchID> iter(watchedFolderMap);

    iter.next();

    while(iter.hasNext())
    {
        qDebug() << "Deleting " << iter.key() << " with " << iter.value();

        fileWatcher.removeWatch(iter.value());

        iter.next();
    }

    delete fsm;
    delete fses;
    delete fileSystemEventListener;
    fsm = nullptr;
    fses = nullptr;
}

void FileMonitoringManager::addFolder(const QString &userFolderPath)
{
    efsw::WatchID watchId = fileWatcher.addWatch(userFolderPath.toStdString(), fileSystemEventListener, false);

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
