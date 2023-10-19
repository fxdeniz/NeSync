#ifndef FILEMONITORINGMANAGER_H
#define FILEMONITORINGMANAGER_H

#include <QObject>
#include <QMultiHash>

#include "FileSystemEventListener.h"
#include "FileSystemEventDb.h"

class FileMonitoringManager : public QObject
{
    Q_OBJECT
public:
    explicit FileMonitoringManager(FileSystemEventDb *fsEventDb, QObject *parent = nullptr);

    bool addFolder(const QString &userFolderPath);
    bool addFile(const QString &userFolderPath, const QString &fileName);

signals:

private slots:
    void slotOnAddEventDetected(const QString &fileName, const QString &dir);
    void slotOnDeleteEventDetected(const QString &fileName, const QString &dir);
    void slotOnModificationEventDetected(const QString &fileName, const QString &dir);
    void slotOnMoveEventDetected(const QString &fileName, const QString &oldFileName, const QString &dir);

private:
    FileSystemEventListener fileSystemEventListener;
    efsw::FileWatcher fileWatcher;
    FileSystemEventDb *eventDb = nullptr;
};

#endif // FILEMONITORINGMANAGER_H
