#ifndef FILEMONITORINGMANAGER_H
#define FILEMONITORINGMANAGER_H

#include <QObject>
#include <QMultiHash>

#include "FileSystemEventListener.h"

class FileMonitoringManager : public QObject
{
    Q_OBJECT
public:
    explicit FileMonitoringManager(QObject *parent = nullptr);

    bool addFolder(const QString &userFolderPath);
    bool addFile(const QString &userFolderPath, const QString &fileName);

signals:

private slots:
    void slotOnAddEventDetected(const QString &fileName, const QString &dir);
    void slotOnDeleteEventDetected(const QString &fileName, const QString &dir);
    void slotOnModificationEventDetected(const QString &fileName, const QString &dir);
    void slotOnMoveEventDetected(const QString &fileName, const QString &oldFileName, const QString &dir);

private:
    QMultiHash<QString, efsw::WatchID> folderDB;
    QMultiHash<QString, QString> fileDB;
    FileSystemEventListener fileSystemEventListener;
    efsw::FileWatcher fileWatcher;
};

#endif // FILEMONITORINGMANAGER_H
