#ifndef FILEMONITORINGMANAGER_H
#define FILEMONITORINGMANAGER_H

#include <QObject>
#include <QTimer>

#include "Backend/FileMonitorSubSystem/FileSystemEventListener.h"
#include "Backend/FileMonitorSubSystem/MonitoredDirDb.h"

#define CONST_MIN_SNAPSHOT_DELAY 3
#define CONST_DEFAULT_SLEEP_DURATION_FOR_RELEASABLE_FILE_CHECK 1

class FileMonitoringManager : public QObject
{
    Q_OBJECT
public:
    explicit FileMonitoringManager(int snapshotDelay = CONST_MIN_SNAPSHOT_DELAY, QObject *parent = nullptr);

    void startMonitoringOn(const QStringList &predictedTargetList);
    int getSnapshotDelay() const;
    void setSnapshotDelay(int newSnapshotDelay);

    const QStringList &getPredictionList() const;
    void setPredictionList(const QStringList &newPredictionList);

public slots:
    void start();

signals:
    void signalPredictedFileNotFound(const QString &pathToFile);
    void signalPredictedFolderNotFound(const QString &pathToFolder);
    void signalPredictionTargetNotRecognized(const QString &pathToTaget);
    void signalUnPredictedFolderDetected(const QString &pathToFolder);
    void signalUnPredictedFileDetected(const QString &pathToFile);
    void signalFileSystemEventAnalysisStarted();
    void signalNewFolderAdded(const QString &pathToFolder);
    void signalFolderDeleted(const QString &pathToFolder);
    void signalFolderMoved(const QString &pathToFolder, const QString &oldFolderName);
    void signalNewFileAdded(const QString &pathToFile);
    void signalFileDeleted(const QString &pathToFile);
    void signalFileModified(const QString &pathToFile);
    void signalFileMoved(const QString &pathToFile, const QString &oldFileName);
    void signalFileMovedAndModified(const QString &pathToFile, const QString &oldFileName);

private:
    void addTargetsFromPredictionList(const QStringList predictedItemList);
    QSet<QString> unPredictedFilesWithRespectTo(QString pathToDirOrFile) const;
    QSet<QString> unPredictedFoldersWithRespectTo(QString pathToDirOrFile) const;
    void addTargetFromDirPath(const QString &dir);
    void addTargetFromFilePath(const QString &pathToFile);
    void releaseNewAddedFolders();
    void releaseDeletedFolders();
    void releaseMovedFolders();
    void releaseNewAddedFiles();
    void releaseDeletedFiles();
    void releaseMovedFiles();
    void releaseModifiedFiles();
    void releaseMovedAndModifiedFiles();

    bool isFileReadyToRelease(const QString currentFilePath,
                              int sleepDuration = CONST_DEFAULT_SLEEP_DURATION_FOR_RELEASABLE_FILE_CHECK);

private slots:
    void slotOnAddEventDetected(const QString &fileName, const QString &dir);
    void slotOnDeleteEventDetected(const QString &fileName, const QString &dir);
    void slotOnModificationEventDetected(const QString &fileName, const QString &dir);
    void slotOnMoveEventDetected(const QString &fileName, const QString &oldFileName, const QString &dir);

    void slotReleaseScheduledEvents();

private:
    efsw::FileWatcher fileWatcher;
    FileSystemEventListener fileSystemEventListener;
    MonitoredDirDb mddb;
    QTimer timer;
    int snapshotDelay = CONST_MIN_SNAPSHOT_DELAY * 1000;
    QStringList predictionList;
};

#endif // FILEMONITORINGMANAGER_H
