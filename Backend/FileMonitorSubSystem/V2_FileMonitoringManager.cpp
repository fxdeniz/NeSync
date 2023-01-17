#include "V2_FileMonitoringManager.h"

#include <QDebug>
#include <QFileInfo>
#include <QRandomGenerator>

V2_FileMonitoringManager::V2_FileMonitoringManager(QObject *parent)
    : QObject{parent}
{
    QObject::connect(&fileSystemEventListener, &FileSystemEventListener::signalAddEventDetected,
                     this, &V2_FileMonitoringManager::slotOnAddEventDetected);

    QObject::connect(&fileSystemEventListener, &FileSystemEventListener::signalDeleteEventDetected,
                     this, &V2_FileMonitoringManager::slotOnDeleteEventDetected);

    QObject::connect(&fileSystemEventListener, &FileSystemEventListener::signalModificationEventDetected,
                     this, &V2_FileMonitoringManager::slotOnModificationEventDetected);

    QObject::connect(&fileSystemEventListener, &FileSystemEventListener::signalMoveEventDetected,
                     this, &V2_FileMonitoringManager::slotOnMoveEventDetected);

    fileWatcher.watch();
}

void V2_FileMonitoringManager::startMonitoringOn(const QStringList &predictedTargetList)
{
    for(const QString &item : predictedTargetList)
    {
        fileWatcher.addWatch(item.toStdString(), &fileSystemEventListener, true);
    }
}

void V2_FileMonitoringManager::slotOnAddEventDetected(const QString &fileName, const QString &dir)
{
    QString currentPath = dir + fileName;
    QFileInfo info(currentPath);
    qDebug() << "addEvent = " << currentPath;
    qDebug() << "";

    if(info.isDir())
    {
        database.addFolder(currentPath);
        long id = QRandomGenerator::system()->generate();

        if(id < 0)
            id = -1 * id;

        database.setEfswIDofFolder(currentPath, id);
        database.setStatusOfFolder(currentPath, FileSystemEventDb::ItemStatus::NewAdded);
    }
    else if(info.isFile())
        database.addFile(currentPath);
}

void V2_FileMonitoringManager::slotOnDeleteEventDetected(const QString &fileName, const QString &dir)
{
    qDebug() << "deleteEvent = " << dir << fileName;
    qDebug() << "";
}

void V2_FileMonitoringManager::slotOnModificationEventDetected(const QString &fileName, const QString &dir)
{
    qDebug() << "updateEvent = " << dir << fileName;
    qDebug() << "";
}

void V2_FileMonitoringManager::slotOnMoveEventDetected(const QString &fileName, const QString &oldFileName, const QString &dir)
{
    qDebug() << "renameEvent (old) -> (new) = " << oldFileName << fileName << dir;
    qDebug() << "";
}
