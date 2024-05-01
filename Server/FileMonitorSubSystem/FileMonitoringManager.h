#ifndef FILEMONITORINGMANAGER_H
#define FILEMONITORINGMANAGER_H

#include <QObject>
#include <efsw/efsw.hpp>
#include "FileSystemEventStore.h"
#include "FileSystemEventListener.h"
#include "FileStorageSubSystem/FileStorageManager.h"

class FileMonitoringManager : public QObject
{
    Q_OBJECT
public:
    explicit FileMonitoringManager(FileStorageManager *fsm,
                                   FileSystemEventStore *fses,
                                   QObject *parent = nullptr);

    ~FileMonitoringManager();

signals:

private slots:
    void slotOnAddEventDetected(const QString &fileOrFolderName, const QString &dir);
    void slotOnDeleteEventDetected(const QString &fileOrFolderName, const QString &dir);
    void slotOnModificationEventDetected(const QString &fileName, const QString &dir);
    void slotOnMoveEventDetected(const QString &fileOrFolderName, const QString &oldFileOrFolderName, const QString &dir);

private:
    FileSystemEventListener fileSystemEventListener;
    efsw::FileWatcher fileWatcher;
    FileStorageManager *fsm;
    FileSystemEventStore *fses;

};

#endif // FILEMONITORINGMANAGER_H
