#include "FileMonitoringManager.h"

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

}

void FileMonitoringManager::slotOnDeleteEventDetected(const QString &fileName, const QString &dir)
{

}

void FileMonitoringManager::slotOnModificationEventDetected(const QString &fileName, const QString &dir)
{

}

void FileMonitoringManager::slotOnMoveEventDetected(const QString &fileName, const QString &oldFileName, const QString &dir)
{

}
