#ifndef FILEMONITORINGMANAGER_H
#define FILEMONITORINGMANAGER_H

#include <QObject>

#include "Backend/FileMonitorSubSystem/FileSystemEventListener.h"
#include "Backend/FileMonitorSubSystem/FileSystemEventDb.h"


class FileMonitoringManager : public QObject
{
    Q_OBJECT
public:
    explicit FileMonitoringManager(const QSqlDatabase &inMemoryDb, QObject *parent = nullptr);
    ~FileMonitoringManager();

    QStringList getPredictionList() const;
    void setPredictionList(const QStringList &newPredictionList);

public slots:
    void start();
    void pauseMonitoring();
    void continueMonitoring();
    void addFolderAtRuntime(const QString &pathToFolder);

signals:
    void signalEventDbUpdated();

private slots:
    void slotOnAddEventDetected(const QString &fileName, const QString &dir);
    void slotOnDeleteEventDetected(const QString &fileName, const QString &dir);
    void slotOnModificationEventDetected(const QString &fileName, const QString &dir);
    void slotOnMoveEventDetected(const QString &fileName, const QString &oldFileName, const QString &dir);

private:
    FileSystemEventDb *database;
    QStringList predictionList;
    FileSystemEventListener fileSystemEventListener;
    efsw::FileWatcher fileWatcher;

};

#endif // FILEMONITORINGMANAGER_H
