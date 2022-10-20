#ifndef V2_FILEMONITORINGMANAGER_H
#define V2_FILEMONITORINGMANAGER_H

#include <QObject>

#include "Backend/FileMonitorSubSystem/FileSystemEventListener.h"
#include "Backend/FileMonitorSubSystem/FileSystemEventDb.h"


class V2_FileMonitoringManager : public QObject
{
    Q_OBJECT
public:
    explicit V2_FileMonitoringManager(QObject *parent = nullptr);
    void startMonitoringOn(const QStringList &predictedTargetList);

private slots:
    void slotOnAddEventDetected(const QString &fileName, const QString &dir);
    void slotOnDeleteEventDetected(const QString &fileName, const QString &dir);
    void slotOnModificationEventDetected(const QString &fileName, const QString &dir);
    void slotOnMoveEventDetected(const QString &fileName, const QString &oldFileName, const QString &dir);

private:
    FileSystemEventDb database;
    FileSystemEventListener fileSystemEventListener;
    efsw::FileWatcher fileWatcher;

};

#endif // V2_FILEMONITORINGMANAGER_H
