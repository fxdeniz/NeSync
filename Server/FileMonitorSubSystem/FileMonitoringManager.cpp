#include "FileMonitoringManager.h"

#include <QFileInfo>

FileMonitoringManager::FileMonitoringManager(QObject *parent)
    : QObject{parent}
{
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
    bool isFolderExist = folderDB.contains(userFolderPath);

    if(isFolderExist)
        return false;

    bool result = false;

    efsw::WatchID watchId = fileWatcher.addWatch(userFolderPath.toStdString(), &fileSystemEventListener, false);

    if(watchId >= efsw::Errors::NoError)
    {
        folderDB.insert(userFolderPath, watchId);
        result = true;
    }

    return result;
}

bool FileMonitoringManager::addFile(const QString &userFolderPath, const QString &fileName)
{
    bool isFolderExist = folderDB.contains(userFolderPath);

    if(!isFolderExist)
        return false;

    bool isFileExist = fileDB.contains(userFolderPath);

    if(isFileExist)
        return false;

    QFileInfo info(userFolderPath + fileName);

    bool isExist = info.exists();
    bool isFile = info.isFile();

    if(!isExist || !isFile)
        return false;

    fileDB.insert(userFolderPath, fileName);

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
