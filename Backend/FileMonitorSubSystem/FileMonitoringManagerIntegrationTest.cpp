#include "FileMonitoringManagerIntegrationTest.h"

FileMonitoringManagerIntegrationTest::FileMonitoringManagerIntegrationTest(QStringList targetPathList, int snapshotDelay, QObject *parent)
    : QObject{parent}
{
    this->dut = new FileMonitoringManager(snapshotDelay);

    QObject::connect(this->dut, &FileMonitoringManager::signalPredictionTargetNotFound,
                     this, &FileMonitoringManagerIntegrationTest::slotOnPredictionTargetNotFound);

    QObject::connect(this->dut, &FileMonitoringManager::signalUnPredictedFolderDetected,
                     this, &FileMonitoringManagerIntegrationTest::slotOnUnPredictedFolderDetected);

    QObject::connect(this->dut, &FileMonitoringManager::signalUnPredictedFileDetected,
                     this, &FileMonitoringManagerIntegrationTest::slotOnUnPredictedFileDetected);

    QObject::connect(this->dut, &FileMonitoringManager::signalFileSystemEventAnalysisStarted,
                     this, &FileMonitoringManagerIntegrationTest::slotOnFileSystemEventAnalysisStarted);

    QObject::connect(this->dut, &FileMonitoringManager::signalNewFolderAdded,
                     this, &FileMonitoringManagerIntegrationTest::slotOnNewFolderAdded);

    QObject::connect(this->dut, &FileMonitoringManager::signalFolderDeleted,
                     this, &FileMonitoringManagerIntegrationTest::slotOnFolderDeleted);

    QObject::connect(this->dut, &FileMonitoringManager::signalFolderMoved,
                     this, &FileMonitoringManagerIntegrationTest::slotOnFolderMoved);

    QObject::connect(this->dut, &FileMonitoringManager::signalNewFileAdded,
                     this, &FileMonitoringManagerIntegrationTest::slotOnNewFileAdded);

    QObject::connect(this->dut, &FileMonitoringManager::signalFileDeleted,
                     this, &FileMonitoringManagerIntegrationTest::slotOnFileDeleted);

    QObject::connect(this->dut, &FileMonitoringManager::signalFileModified,
                     this, &FileMonitoringManagerIntegrationTest::slotOnFileModified);

    QObject::connect(this->dut, &FileMonitoringManager::signalFileMoved,
                     this, &FileMonitoringManagerIntegrationTest::slotOnFileMoved);

    QObject::connect(this->dut, &FileMonitoringManager::signalFileMovedAndModified,
                     this, &FileMonitoringManagerIntegrationTest::slotOnFileMovedAndModified);

    this->dut->startMonitoringOn(targetPathList);

}

FileMonitoringManagerIntegrationTest::~FileMonitoringManagerIntegrationTest()
{
    if(this->dut != nullptr)
        delete this->dut;
}

void FileMonitoringManagerIntegrationTest::slotOnPredictionTargetNotFound(const QString &pathToTaget)
{
    qDebug() << "slotOnPredictionTargetNotFound = " << pathToTaget;
}

void FileMonitoringManagerIntegrationTest::slotOnUnPredictedFolderDetected(const QString &pathToFolder)
{
    qDebug() << "slotOnUnPredictedFolderDetected = " << pathToFolder;
}

void FileMonitoringManagerIntegrationTest::slotOnUnPredictedFileDetected(const QString &pathToFile)
{
    qDebug() << "slotOnUnPredictedFileDetected = " << pathToFile;
}

void FileMonitoringManagerIntegrationTest::slotOnFileSystemEventAnalysisStarted()
{
    qDebug() << "slotOnFileSystemEventAnalysisStarted(void)";
}

void FileMonitoringManagerIntegrationTest::slotOnNewFolderAdded(const QString &pathToFolder)
{
    qDebug() << "slotOnNewFolderAdded = " << pathToFolder;
}

void FileMonitoringManagerIntegrationTest::slotOnFolderDeleted(const QString &pathToFolder)
{
    qDebug() << "slotOnFolderDeleted = " << pathToFolder;
}

void FileMonitoringManagerIntegrationTest::slotOnFolderMoved(const QString &pathToFolder, const QString &oldFolderName)
{
    qDebug() << "slotOnFolderMoved()    pathToFolder = " << pathToFolder << "    oldFolderName = " << oldFolderName;
}

void FileMonitoringManagerIntegrationTest::slotOnNewFileAdded(const QString &pathToFile)
{
    qDebug() << "slotOnNewFileAdded = " << pathToFile;
}

void FileMonitoringManagerIntegrationTest::slotOnFileDeleted(const QString &pathToFile)
{
    qDebug() << "slotOnFileDeleted = " << pathToFile;
}

void FileMonitoringManagerIntegrationTest::slotOnFileModified(const QString &pathToFile)
{
    qDebug() << "slotOnFileModified = " << pathToFile;
}

void FileMonitoringManagerIntegrationTest::slotOnFileMoved(const QString &pathToFile, const QString &oldFileName)
{
    qDebug() << "slotOnFileMoved()    pathToFile = " << pathToFile << "    oldFileName = " << oldFileName;
}

void FileMonitoringManagerIntegrationTest::slotOnFileMovedAndModified(const QString &pathToFile, const QString &oldFileName)
{
    qDebug() << "slotOnFileMovedAndModified()    pathToFile = " << pathToFile << "    oldFileName = " << oldFileName;
}
