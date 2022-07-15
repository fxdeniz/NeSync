#ifndef FILEMONITORINGMANAGERINTEGRATIONTEST_H
#define FILEMONITORINGMANAGERINTEGRATIONTEST_H

#include <QObject>

#include "Backend/FileMonitorSubSystem/FileMonitoringManager.h"

class FileMonitoringManagerIntegrationTest : public QObject
{
    Q_OBJECT
public:
    explicit FileMonitoringManagerIntegrationTest(QStringList targetPathList,
                                                  int snapshotDelay = CONST_MIN_SNAPSHOT_DELAY,
                                                  QObject *parent = nullptr);

    virtual ~FileMonitoringManagerIntegrationTest();

private slots:
    void slotOnPredictionTargetNotFound(const QString &pathToTaget);
    void slotOnUnPredictedFolderDetected(const QString &pathToFolder);
    void slotOnUnPredictedFileDetected(const QString &pathToFile);
    void slotOnFileSystemEventAnalysisStarted();
    void slotOnNewFolderAdded(const QString &pathToFolder);
    void slotOnFolderDeleted(const QString &pathToFolder);
    void slotOnFolderMoved(const QString &pathToFolder, const QString &oldFolderName);
    void slotOnNewFileAdded(const QString &pathToFile);
    void slotOnFileDeleted(const QString &pathToFile);
    void slotOnFileModified(const QString &pathToFile);
    void slotOnFileMoved(const QString &pathToFile, const QString &oldFileName);
    void slotOnFileMovedAndModified(const QString &pathToFile, const QString &oldFileName);

private:
    FileMonitoringManager *dut = nullptr;

};

#endif // FILEMONITORINGMANAGERINTEGRATIONTEST_H
