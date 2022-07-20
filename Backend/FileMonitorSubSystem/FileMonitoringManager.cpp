#include "FileMonitoringManager.h"

#include <QCryptographicHash>
#include <QDirIterator>
#include <QFileInfo>
#include <QThread>
#include <QFile>
#include <QDir>

FileMonitoringManager::FileMonitoringManager(int snapshotDelay, QObject *parent)
    : QObject{parent}
{
    QObject::connect(&this->fileSystemEventListener, &FileSystemEventListener::signalAddEventDetected,
                     this, &FileMonitoringManager::slotOnAddEventDetected);

    QObject::connect(&this->fileSystemEventListener, &FileSystemEventListener::signalDeleteEventDetected,
                     this, &FileMonitoringManager::slotOnDeleteEventDetected);

    QObject::connect(&this->fileSystemEventListener, &FileSystemEventListener::signalModificationEventDetected,
                     this, &FileMonitoringManager::slotOnModificationEventDetected);

    QObject::connect(&this->fileSystemEventListener, &FileSystemEventListener::signalMoveEventDetected,
                     this, &FileMonitoringManager::slotOnMoveEventDetected);

    QObject::connect(timer, &QTimer::timeout,
                     this, &FileMonitoringManager::slotReleaseScheduledEvents);

    timer = new QTimer(this);
    this->setSnapshotDelay(snapshotDelay);
    this->fileWatcher.watch();
}

FileMonitoringManager::FileMonitoringManager(QTimer *_timer, int snapshotDelay, QObject *parent)
    : QObject{parent}
{
    QObject::connect(&this->fileSystemEventListener, &FileSystemEventListener::signalAddEventDetected,
                     this, &FileMonitoringManager::slotOnAddEventDetected);

    QObject::connect(&this->fileSystemEventListener, &FileSystemEventListener::signalDeleteEventDetected,
                     this, &FileMonitoringManager::slotOnDeleteEventDetected);

    QObject::connect(&this->fileSystemEventListener, &FileSystemEventListener::signalModificationEventDetected,
                     this, &FileMonitoringManager::slotOnModificationEventDetected);

    QObject::connect(&this->fileSystemEventListener, &FileSystemEventListener::signalMoveEventDetected,
                     this, &FileMonitoringManager::slotOnMoveEventDetected);

    QObject::connect(_timer, &QTimer::timeout,
                     this, &FileMonitoringManager::slotReleaseScheduledEvents);

    this->timer = _timer;
    this->setSnapshotDelay(snapshotDelay);
    this->fileWatcher.watch();
}

void FileMonitoringManager::startMonitoringOn(const QStringList &predictedTargetList)
{
    auto queryResult = this->mddb.efswWatchIDList();

    for(qlonglong currentID : queryResult)
        this->fileWatcher.removeWatch(currentID);

    this->mddb.resetDb();
    this->addTargetsFromPredictionList(predictedTargetList);
}

int FileMonitoringManager::getSnapshotDelay() const
{
    return this->snapshotDelay;
}

void FileMonitoringManager::setSnapshotDelay(int newSnapshotDelay)
{
    if(newSnapshotDelay > CONST_MIN_SNAPSHOT_DELAY)
        this->snapshotDelay = newSnapshotDelay * 1000;
}

const QStringList &FileMonitoringManager::getPredictionList() const
{
    return predictionList;
}

void FileMonitoringManager::setPredictionList(const QStringList &newPredictionList)
{
    predictionList = newPredictionList;
}

void FileMonitoringManager::start()
{
    startMonitoringOn(getPredictionList());
}

void FileMonitoringManager::addTargetsFromPredictionList(const QStringList predictedItemList)
{
    QSet<QString> predictedFiles, predictedFolders;
    QSet<QString> unPredictedFiles, unPredictedFolders;

    for(const QString &currentPath : predictedItemList)
    {
        auto standarizedPath = QDir::fromNativeSeparators(currentPath);

        QFileInfo info(currentPath);

        if(info.exists())
        {
            if(info.isFile())
            {
                predictedFiles.insert(standarizedPath);
                this->addTargetFromFilePath(standarizedPath);

                unPredictedFiles += this->unPredictedFilesWithRespectTo(standarizedPath);
                unPredictedFolders += this->unPredictedFoldersWithRespectTo(standarizedPath);
            }
            else if(info.isDir())
            {
                predictedFolders.insert(standarizedPath);
                this->addTargetFromDirPath(standarizedPath);

                unPredictedFolders += this->unPredictedFoldersWithRespectTo(standarizedPath);
                unPredictedFiles += this->unPredictedFilesWithRespectTo(standarizedPath);
            }
        }
        else
            emit signalPredictionTargetNotFound(currentPath);
    }

    unPredictedFiles -= predictedFiles;
    unPredictedFolders -= predictedFolders;

    QSet<QString> unPredictedResult = unPredictedFiles + unPredictedFolders;

    for(const QString &currentPath : qAsConst(unPredictedResult))
    {
        QFileInfo info(currentPath);

        if(info.exists() && info.isFile())
        {
            this->addTargetFromFilePath(currentPath);
            emit signalUnPredictedFileDetected(currentPath);
        }
        else if(info.exists() && info.isDir())
        {
            this->addTargetFromDirPath(currentPath);
            emit signalUnPredictedFolderDetected(currentPath);
        }
    }
}

QSet<QString> FileMonitoringManager::unPredictedFilesWithRespectTo(QString pathToDirOrFile) const
{
    QSet<QString> result;

    auto dir = QFileInfo(pathToDirOrFile).dir();
    dir.setFilter(QDir::Filter::Files);

    QDirIterator iterator(dir, QDirIterator::IteratorFlag::Subdirectories);

    while(iterator.hasNext())
        result.insert(iterator.next());

    auto standardizedPath = QDir::fromNativeSeparators(pathToDirOrFile);
    result.remove(standardizedPath);

    return result;
}

QSet<QString> FileMonitoringManager::unPredictedFoldersWithRespectTo(QString pathToDirOrFile) const
{
    QSet<QString> result;

    auto dir = QFileInfo(pathToDirOrFile).dir();
    dir.setFilter(QDir::Filter::Dirs | QDir::Filter::NoDotAndDotDot);

    QDirIterator iterator(dir, QDirIterator::IteratorFlag::Subdirectories);

    while(iterator.hasNext())
        result.insert(iterator.next());

    auto standardizedPath = QDir::fromNativeSeparators(pathToDirOrFile);
    result.remove(standardizedPath);

    return result;
}

void FileMonitoringManager::addTargetFromDirPath(const QString &dir)
{
    bool isDirExist = this->mddb.isDirExistByPath(dir);

    if(!isDirExist)
    {
        auto _dir = this->mddb.standardizeDir(dir);
        auto id = this->fileWatcher.addWatch(_dir.toStdString(), &this->fileSystemEventListener, true);

        if(id > 0)
            this->mddb.addDir(_dir, id);
    }
}

void FileMonitoringManager::addTargetFromFilePath(const QString &pathToFile)
{
    QFileInfo info(pathToFile);
    auto dir = info.absolutePath();
    auto fileName = info.fileName();

    bool isFileExist = this->mddb.isFileExistInDir(fileName, dir);

    if(!isFileExist)
    {
        bool isDirExist = this->mddb.isDirExistByPath(dir);

        if(isDirExist)
            this->mddb.addFileToDir(fileName, dir);
        else
        {
            auto _dir = this->mddb.standardizeDir(dir);
            auto id = this->fileWatcher.addWatch(_dir.toStdString(), &this->fileSystemEventListener, true);

            if(id > 0)
            {
                this->mddb.addDir(dir, id);
                this->mddb.addFileToDir(fileName, dir);
            }
        }
    }
}

void FileMonitoringManager::releaseNewAddedFolders()
{
    auto newAddedFolderList = this->mddb.scheduledDirList(MonitoredDirDb::MonitoredItemState::NewAdded);

    for(const QString &currentFolderPath : newAddedFolderList)
    {
        QFileInfo info(currentFolderPath);

        bool isFolderExist = info.exists();
        if(!isFolderExist)
            continue;

        emit signalNewFolderAdded(currentFolderPath);

        this->mddb.unScheduleDir(currentFolderPath);
    }
}

void FileMonitoringManager::releaseDeletedFolders()
{
    auto deletedFolderList = this->mddb.scheduledDirList(MonitoredDirDb::MonitoredItemState::Deleted);

    for(const QString &currentFolderPath : deletedFolderList)
    {
        QFileInfo info(currentFolderPath);

        bool isFolderExist = info.exists();
        if(isFolderExist)
            continue;

        auto containedFileList = this->mddb.fileListInDir(currentFolderPath);

        for(const QString &currentFileName : containedFileList)
        {
            QFileInfo info(currentFolderPath + currentFileName);

            bool isFileExist = info.exists();
            if(isFileExist)
                continue;

            emit signalFileDeleted(currentFolderPath + currentFileName);
        }

        emit signalFolderDeleted(currentFolderPath);

        auto id = this->mddb.efswWatchIDofDir(currentFolderPath);
        this->fileWatcher.removeWatch(id);
        this->mddb.removeDir(currentFolderPath);
    }
}

void FileMonitoringManager::releaseMovedFolders()
{
    auto movedFolderList = this->mddb.scheduledDirList(MonitoredDirDb::MonitoredItemState::Moved);

    for(const QString &currentFolderPath : movedFolderList)
    {
        QFileInfo info(currentFolderPath);

        bool isFolderExist = info.exists();
        if(!isFolderExist)
            continue;

        auto oldFolderName = this->mddb.oldNameOfDir(currentFolderPath);
        emit signalFolderMoved(currentFolderPath, oldFolderName);

        this->mddb.unScheduleDir(currentFolderPath);
    }
}

void FileMonitoringManager::releaseNewAddedFiles()
{
    auto newAddedFileList = this->mddb.scheduledFileList(MonitoredDirDb::MonitoredItemState::NewAdded);

    for(const QString &currentFilePath : newAddedFileList)
    {
        bool isFileReady = this->isFileReadyToRelease(currentFilePath);

        if(!isFileReady)
            continue;

        QFileInfo info(currentFilePath);
        emit signalNewFileAdded(currentFilePath);
        this->mddb.unScheduleFileInDir(info.fileName(), info.absolutePath());
    }
}

void FileMonitoringManager::releaseDeletedFiles()
{
    auto deletedFileList = this->mddb.scheduledFileList(MonitoredDirDb::MonitoredItemState::Deleted);

    for(const QString &currentFilePath : deletedFileList)
    {
        QFileInfo info(currentFilePath);

        bool isFileExist = info.exists();
        if(isFileExist)
            continue;

        emit signalFileDeleted(currentFilePath);

        this->mddb.removeFileFromDir(info.fileName(), info.absolutePath());
    }
}

void FileMonitoringManager::releaseMovedFiles()
{
    auto movedFileList = this->mddb.scheduledFileList(MonitoredDirDb::MonitoredItemState::Moved);

    for(const QString &currentFilePath : movedFileList)
    {
        bool isFileReady = this->isFileReadyToRelease(currentFilePath);

        if(!isFileReady)
            continue;

        QFileInfo info(currentFilePath);

        auto oldFileName = this->mddb.oldFileNameOfFileInDir(info.fileName(), info.absolutePath());
        emit signalFileMoved(currentFilePath, oldFileName);
        this->mddb.unScheduleFileInDir(info.fileName(), info.absolutePath());
    }
}

void FileMonitoringManager::releaseModifiedFiles()
{
    auto modifiedFileList = this->mddb.scheduledFileList(MonitoredDirDb::MonitoredItemState::Modified);

    for(const QString &currentFilePath : modifiedFileList)
    {
        bool isFileReady = this->isFileReadyToRelease(currentFilePath);

        if(!isFileReady)
            continue;

        QFileInfo info(currentFilePath);
        emit signalFileModified(currentFilePath);
        this->mddb.unScheduleFileInDir(info.fileName(), info.absolutePath());
    }
}

void FileMonitoringManager::releaseMovedAndModifiedFiles()
{
    auto movedAndmodifiedFileList = this->mddb.scheduledFileList(MonitoredDirDb::MonitoredItemState::MovedAndModified);

    for(const QString &currentFilePath : movedAndmodifiedFileList)
    {
        bool isFileReady = this->isFileReadyToRelease(currentFilePath);

        if(!isFileReady)
            continue;

        QFileInfo info(currentFilePath);

        auto oldFileName = this->mddb.oldFileNameOfFileInDir(info.fileName(), info.absolutePath());
        emit signalFileMovedAndModified(currentFilePath, oldFileName);
        this->mddb.unScheduleFileInDir(info.fileName(), info.absolutePath());
    }
}

bool FileMonitoringManager::isFileReadyToRelease(const QString currentFilePath, int sleepDuration)
{
    bool result = false;
    QFileInfo info(currentFilePath);

    if(sleepDuration < CONST_DEFAULT_SLEEP_DURATION_FOR_RELEASABLE_FILE_CHECK)
        sleepDuration = CONST_DEFAULT_SLEEP_DURATION_FOR_RELEASABLE_FILE_CHECK;

    auto beforeTimestamp = info.lastModified();
    QThread::currentThread()->sleep(sleepDuration);
    info.refresh();
    auto afterTimestamp = info.lastModified();

    bool isFileExist = info.exists();
    bool isModificationTimeSame = (beforeTimestamp == afterTimestamp);

    if(isFileExist && isModificationTimeSame)
        result = true;

    return result;
}

void FileMonitoringManager::slotOnAddEventDetected(const QString &fileName, const QString &dir)
{
#ifdef DEBUG_FSM_SLOTS
    qDebug() << "\t FileMonitoringManager::slotOnAddEventDetected() \t in = " << QThread::currentThread();
    qDebug() << "\t\t fileName = " << fileName;
    qDebug() << "\t\t dir = " << dir;
#endif

    this->timer->stop();
    QString _dir = this->mddb.standardizeDir(dir);

    QFileInfo info(_dir + fileName);

    if(info.isFile()) // If new added item is a file.
    {
        if(!info.isHidden())
        {
            this->addTargetFromFilePath(_dir + fileName);
            this->mddb.updateEventTimestampOfFileInDir(fileName, dir, QDateTime::currentDateTime());
            this->mddb.scheduleFileInDirAs(fileName, dir, MonitoredDirDb::MonitoredItemState::NewAdded);
        }
    }
    else if(info.isDir()) // If new added item is a folder.
    {
        auto dirPath =  QDir::toNativeSeparators(_dir + fileName);
        this->addTargetFromDirPath(dirPath);
        this->mddb.updateEventTimestampOfDir(dirPath, QDateTime::currentDateTime());
        this->mddb.scheduleDirAs(dirPath, MonitoredDirDb::MonitoredItemState::NewAdded);
    }

    this->timer->start(this->getSnapshotDelay());
}

void FileMonitoringManager::slotOnDeleteEventDetected(const QString &fileName, const QString &dir)
{
#ifdef DEBUG_FSM_SLOTS
    qDebug() << "\t FileMonitoringManager::slotOnDeleteEventDetected() \t in = " << QThread::currentThread();
    qDebug() << "\t\t fileName = " << fileName;
    qDebug() << "\t\t dir = " << dir;
#endif

    this->timer->stop();

    bool isFile = this->mddb.isFileExistInDir(fileName, dir);
    auto _dir = this->mddb.standardizeDir(dir);
    bool isDir = this->mddb.isDirExistByPath(_dir + fileName);

    if(isFile) // If deleted item is a file.
    {
        this->mddb.updateEventTimestampOfFileInDir(fileName, dir, QDateTime::currentDateTime());
        this->mddb.scheduleFileInDirAs(fileName, dir, MonitoredDirDb::MonitoredItemState::Deleted);
    }
    else if(isDir) // If deleted item is a folder.
    {
        auto dirPath = _dir + fileName;

        this->mddb.updateEventTimestampOfDir(dirPath, QDateTime::currentDateTime());
        this->mddb.scheduleDirAs(dirPath, MonitoredDirDb::MonitoredItemState::Deleted);
    }

    this->timer->start(this->getSnapshotDelay());
}

void FileMonitoringManager::slotOnModificationEventDetected(const QString &fileName, const QString &dir)
{
#ifdef DEBUG_FSM_SLOTS
    qDebug() << "\t FileMonitoringManager::slotOnModificationEventDetected() \t in = " << QThread::currentThread();
    qDebug() << "\t\t fileName = " << fileName;
    qDebug() << "\t\t dir = " << dir;
#endif

    this->timer->stop();

    bool isFile = this->mddb.isFileExistInDir(fileName, dir);

    // Check isFile from databse record for catching case where modified file is moved immediately.
    if(isFile) // If modified item is a file.
    {
        auto currentState = this->mddb.stateOfFileInDir(fileName, dir);

        // Do not count modifications if files is new added.
        if(currentState != MonitoredDirDb::MonitoredItemState::NewAdded)
        {
            if(currentState == MonitoredDirDb::MonitoredItemState::Moved ||
               currentState == MonitoredDirDb::MonitoredItemState::MovedAndModified)
            {
                this->mddb.scheduleFileInDirAs(fileName, dir, MonitoredDirDb::MonitoredItemState::MovedAndModified);
            }
            else
                this->mddb.scheduleFileInDirAs(fileName, dir, MonitoredDirDb::MonitoredItemState::Modified);
        }

        this->mddb.updateEventTimestampOfFileInDir(fileName, dir, QDateTime::currentDateTime());
    }

    this->timer->start(this->getSnapshotDelay());
}

void FileMonitoringManager::slotOnMoveEventDetected(const QString &fileName, const QString &oldFileName, const QString &dir)
{
#ifdef DEBUG_FSM_SLOTS
    qDebug() << "\t FileMonitoringManager::slotOnMoveEventDetected() \t\t in = " << QThread::currentThread();
    qDebug() << "\t\t fileName = " << fileName;
    qDebug() << "\t\t oldFileName = " << oldFileName;
    qDebug() << "\t\t dir = " << dir;
#endif

    this->timer->stop();

    auto _dir = this->mddb.standardizeDir(dir);
    QFileInfo info(_dir + fileName);

    if(info.isFile())
    {
        bool isOldFileExist = this->mddb.isFileExistInDir(oldFileName, dir);
        bool isNewFileExist = this->mddb.isFileExistInDir(fileName, dir);

        if(isOldFileExist && !isNewFileExist)
        {
            this->mddb.updateOldFileNameOfFileInDir(oldFileName, oldFileName, dir);
            this->mddb.renameFileInDir(oldFileName, fileName, dir);

            auto currentState = this->mddb.stateOfFileInDir(fileName, dir);

            // Do not count renames (moves) if files is new added.
            if(currentState != MonitoredDirDb::MonitoredItemState::NewAdded)
            {
                if(currentState == MonitoredDirDb::MonitoredItemState::Modified ||
                    currentState == MonitoredDirDb::MonitoredItemState::MovedAndModified)
                {
                    this->mddb.scheduleFileInDirAs(fileName, dir, MonitoredDirDb::MonitoredItemState::MovedAndModified);
                }
                else
                {
                    this->mddb.scheduleFileInDirAs(fileName, dir, MonitoredDirDb::MonitoredItemState::Moved);
                }
            }
//            else
//            {
//                // If newly added file overwrites existing file then, consider that file modified
//                this->mddb.scheduleFileInDirAs(fileName, dir, MonitoredDirDb::MonitoredItemState::Modified);
//            }

            this->mddb.updateEventTimestampOfFileInDir(fileName, dir, QDateTime::currentDateTime());
        }
        else if(isOldFileExist && isNewFileExist) // Ghost file cleanup
        {
            this->mddb.removeFileFromDir(oldFileName, _dir);
        }
    }
    else if(info.isDir())
    {
        bool isOldDirExist = this->mddb.isDirExistByPath(_dir + oldFileName);
        bool isNewDirExist = this->mddb.isDirExistByPath(_dir + fileName);

        if(isOldDirExist && !isNewDirExist)
        {
            auto currentState = this->mddb.stateOfDir(_dir + oldFileName);

            // Do not count renames (moves) if folder is new added.
            if(currentState != MonitoredDirDb::MonitoredItemState::NewAdded)
                this->mddb.scheduleDirAs(_dir + oldFileName, MonitoredDirDb::MonitoredItemState::Moved);

            auto dirPath = _dir + fileName;

            this->mddb.renameDir(_dir + oldFileName, dirPath);
            this->mddb.updateOldNameOfDir(oldFileName, dirPath);
            this->mddb.updateEventTimestampOfDir(_dir + fileName, QDateTime::currentDateTime());
        }
    }

    this->timer->start(this->getSnapshotDelay());
}

void FileMonitoringManager::slotReleaseScheduledEvents()
{
#ifdef DEBUG_FSM_SLOTS
    qDebug() << "-----> FileMonitoringManager::slotReleaseScheduledEvents() \t in = " << QThread::currentThread();
#endif

    this->timer->stop();
    emit signalFileSystemEventAnalysisStarted();

    this->releaseNewAddedFolders();
    this->releaseDeletedFolders();
    this->releaseMovedFolders();
    this->releaseNewAddedFiles();
    this->releaseDeletedFiles();
    this->releaseMovedFiles();
    this->releaseModifiedFiles();
    this->releaseMovedAndModifiedFiles();

//    bool isStillContainScheduledFiles = this->mddb.hasContainScheduledFiles();

//    if(!isStillContainScheduledFiles)
//        this->timer.stop();
//    else
    //        this->timer.start(this->getSnapshotDelay());
}
