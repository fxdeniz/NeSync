#include "FileMonitoringManager.h"

#include "FileStorageSubSystem/FileStorageManager.h"
#include "Utility/JsonDtoFormat.h"

#include <QDir>
#include <QDebug>
#include <QFileInfo>
#include <QDirIterator>
#include <QRandomGenerator>

FileMonitoringManager::FileMonitoringManager(const QSqlDatabase &inMemoryDb, QObject *parent)
    : QObject{parent}
{
    database = new FileSystemEventDb(inMemoryDb);

    QObject::connect(&fileSystemEventListener, &FileSystemEventListener::signalAddEventDetected,
                     this, &FileMonitoringManager::slotOnAddEventDetected);

    QObject::connect(&fileSystemEventListener, &FileSystemEventListener::signalDeleteEventDetected,
                     this, &FileMonitoringManager::slotOnDeleteEventDetected);

    QObject::connect(&fileSystemEventListener, &FileSystemEventListener::signalModificationEventDetected,
                     this, &FileMonitoringManager::slotOnModificationEventDetected);

    QObject::connect(&fileSystemEventListener, &FileSystemEventListener::signalMoveEventDetected,
                     this, &FileMonitoringManager::slotOnMoveEventDetected);

    fileWatcher.watch();
}

FileMonitoringManager::~FileMonitoringManager()
{
    delete database;
}

QStringList FileMonitoringManager::getPredictionList() const
{
    return predictionList;
}

void FileMonitoringManager::setPredictionList(const QStringList &newPredictionList)
{
    predictionList = newPredictionList;
}

void FileMonitoringManager::start()
{
    auto fsm = FileStorageManager::instance();

    for(const QString &item : getPredictionList())
    {
        QFileInfo info(item);

        if(info.exists())
        {
            QString folderPath;

            if(info.isDir())
                folderPath = item;
            else
                folderPath = info.absolutePath();

            folderPath = QDir::toNativeSeparators(folderPath);

            if(!folderPath.endsWith(QDir::separator()))
                folderPath.append(QDir::separator());

            efsw::WatchID watchId = fileWatcher.addWatch(folderPath.toStdString(), &fileSystemEventListener, false);

            if(watchId > 0) // Successfully started monitoring folder
            {
                database->addFolder(folderPath);
                database->setEfswIDofFolder(folderPath, watchId);
            }

            if(info.isFile()) // Add files in any case
                database->addFile(item);
        }
        else
        {
            QJsonObject folderJson = fsm->getFolderJsonByUserPath(item);
            QJsonObject fileJson = fsm->getFileJsonByUserPath(item);

            if(folderJson[JsonKeys::IsExist].toBool()) // If folder is missing
            {
                database->addFolder(item);
                database->setStatusOfFolder(item, FileSystemEventDb::ItemStatus::Missing);
            }
            else if(fileJson[JsonKeys::IsExist].toBool())
            {
                database->addFile(item);
                database->setStatusOfFile(item, FileSystemEventDb::ItemStatus::Missing);
            }
            else
                database->addMonitoringError(item, "Initialization", efsw::Error::FileNotFound);
        }
    }

    // Discover not predicted folders & files
    QStringList queryResult = database->getMonitoredFolderPathList();
    for(const QString &queryItem : queryResult)
    {
        QDir dir(queryItem);
        dir.setFilter(QDir::Filter::Dirs | QDir::Filter::NoDotAndDotDot);
        QDirIterator dirIterator(dir, QDirIterator::IteratorFlag::Subdirectories);

        // Insert folders
        while(dirIterator.hasNext())
        {
            QFileInfo info = dirIterator.nextFileInfo();
            QString candidateFolderPath = QDir::toNativeSeparators(info.absoluteFilePath());
            if(!candidateFolderPath.endsWith(QDir::separator()))
                candidateFolderPath.append(QDir::separator());

            QJsonObject folderJson = fsm->getFolderJsonByUserPath(candidateFolderPath);

            bool isFolderMonitored = database->isFolderExist(candidateFolderPath);
            bool isFolderFrozen = folderJson[JsonKeys::Folder::IsFrozen].toBool();

            if(!isFolderMonitored && !isFolderFrozen)
            {
                efsw::WatchID watchId = fileWatcher.addWatch(candidateFolderPath.toStdString(), &fileSystemEventListener, false);

                if(watchId <= 0) // Couldn't start monitoring folder successfully
                    database->addMonitoringError(candidateFolderPath, "Discovery", watchId);
                else // Successfully started monitoring folder
                {
                    database->addFolder(candidateFolderPath);
                    database->setEfswIDofFolder(candidateFolderPath, watchId);
                    database->setStatusOfFolder(candidateFolderPath, FileSystemEventDb::ItemStatus::NewAdded);
                }
            }
        }

        dir.setPath(queryItem);
        dir.setFilter(QDir::Filter::Files | QDir::Filter::NoDotAndDotDot);
        QDirIterator fileIterator(dir, QDirIterator::IteratorFlag::Subdirectories);

        // Now insert files
        while(fileIterator.hasNext())
        {
            QFileInfo info = fileIterator.nextFileInfo();
            QString candidateFilePath = QDir::toNativeSeparators(info.absoluteFilePath());
            QJsonObject fileJson = fsm->getFileJsonByUserPath(candidateFilePath);

            bool isFileMonitored = database->isFileExist(candidateFilePath);
            bool isFileFrozen = fileJson[JsonKeys::File::IsFrozen].toBool();

            if(!isFileMonitored && !isFileFrozen)
            {
                database->addFile(candidateFilePath);
                database->setStatusOfFile(candidateFilePath, FileSystemEventDb::ItemStatus::NewAdded);
            }
        }
    }

    emit signalEventDbUpdated();
}

void FileMonitoringManager::pauseMonitoring()
{
    fileSystemEventListener.blockSignals(true);
}

void FileMonitoringManager::continueMonitoring()
{
    fileSystemEventListener.blockSignals(false);
}

void FileMonitoringManager::addTargetAtRuntime(const QString &pathToFileOrFolder)
{
    QFileInfo info(pathToFileOrFolder);
    if(info.isDir())
        slotOnAddEventDetected("", pathToFileOrFolder);
    else if(info.isFile())
        slotOnAddEventDetected(info.fileName(), info.absolutePath());
}

void FileMonitoringManager::stopMonitoringTarget(const QString &pathToFileOrFolder)
{
    QFileInfo info(pathToFileOrFolder);

    if(info.isFile())
        database->deleteFile(pathToFileOrFolder);
    else if(info.isDir())
    {
        bool isFolderMonitored = database->isFolderExist(pathToFileOrFolder);

        if(isFolderMonitored)
        {
            QList<efsw::WatchID> result = database->getEfswIDListOfFolderTree(pathToFileOrFolder);

            for(const efsw::WatchID watchId : result)
                fileWatcher.removeWatch(watchId);

            database->deleteFolder(pathToFileOrFolder);
        }
    }
}

void FileMonitoringManager::slotOnAddEventDetected(const QString &fileName, const QString &dir)
{
    QString _dir = dir;

    if(!_dir.endsWith(QDir::separator()))
        _dir.append(QDir::separator());

    QString currentPath = QDir::toNativeSeparators(_dir + fileName);
    QFileInfo info(currentPath);
    qDebug() << "addEvent = " << currentPath;
    qDebug() << "";

    if(info.isDir())
    {
        if(!currentPath.endsWith(QDir::separator()))
            currentPath.append(QDir::separator());

        bool isFolderMonitored = database->isFolderExist(currentPath);

        auto fsm = FileStorageManager::instance();
        QJsonObject folderJson = fsm->getFolderJsonByUserPath(currentPath);
        bool isFolderPersists = folderJson[JsonKeys::IsExist].toBool();
        bool isFolderFrozen = folderJson[JsonKeys::Folder::IsFrozen].toBool();

        if(!isFolderFrozen) // Only monitor active (un-frozen) folders
        {
            efsw::WatchID watchId = fileWatcher.addWatch(currentPath.toStdString(), &fileSystemEventListener, false);

            if(watchId <= 0) // Coludn't start monitoring folder
                database->addMonitoringError(currentPath, "AddEvent", watchId);
            else    // Successfully started monitoring folder
            {
                if(!isFolderMonitored)
                    database->addFolder(currentPath);

                database->setEfswIDofFolder(currentPath, watchId);

                if(!isFolderPersists)
                    database->setStatusOfFolder(currentPath, FileSystemEventDb::ItemStatus::NewAdded);
                else
                {
                    if(fileSystemEventListener.signalsBlocked()) // If adding folder at runtime
                        database->setStatusOfFolder(currentPath, FileSystemEventDb::ItemStatus::Monitored);
                    else
                        database->setStatusOfFolder(currentPath, FileSystemEventDb::ItemStatus::Updated);
                }
            }

            if(!fileSystemEventListener.signalsBlocked()) // If monitoring paused, do not trigger ui events
                emit signalEventDbUpdated();
        }
    }
    else if(info.isFile() && !info.isHidden()) // Only accept real files
    {
        auto status = FileSystemEventDb::ItemStatus::Monitored;

        auto fsm = FileStorageManager::instance();
        QJsonObject fileJson = fsm->getFileJsonByUserPath(currentPath);
        bool isFilePersists = fileJson[JsonKeys::IsExist].toBool();
        bool isFileFrozen = fileJson[JsonKeys::File::IsFrozen].toBool();

        if(!isFileFrozen) // Only monitor active (un-frozen) files
        {
            if(!isFilePersists)
                status = FileSystemEventDb::ItemStatus::NewAdded;
            else
            {
                if(fileSystemEventListener.signalsBlocked()) // If adding file at runtime
                    status = FileSystemEventDb::ItemStatus::Monitored;
                else
                    status = FileSystemEventDb::ItemStatus::Updated;
            }

            database->addFile(currentPath);
            database->setStatusOfFile(currentPath, status);

            if(!fileSystemEventListener.signalsBlocked()) // If monitoring paused, do not trigger ui events
                emit signalEventDbUpdated();
        }
    }
}

void FileMonitoringManager::slotOnDeleteEventDetected(const QString &fileName, const QString &dir)
{
    qDebug() << "deleteEvent = " << dir << fileName;
    qDebug() << "";

    QString currentPath = QDir::toNativeSeparators(dir + fileName);
    FileSystemEventDb::ItemStatus currentStatus;

    if(database->isFolderExist(currentPath)) // When folder deleted
    {
        efsw::WatchID watchId = database->getEfswIDofFolder(currentPath);
        currentStatus = database->getStatusOfFolder(currentPath);

        if(currentStatus == FileSystemEventDb::ItemStatus::NewAdded) // Remove new added folders since they're temporary
            database->deleteFolder(currentPath);
        else
            database->setStatusOfFolder(currentPath, FileSystemEventDb::ItemStatus::Deleted);

        fileWatcher.removeWatch(watchId);

        emit signalEventDbUpdated();
    }
    else if(database->isFileExist(currentPath)) // When file deleted
    {
        currentStatus = database->getStatusOfFile(currentPath);

        if(currentStatus == FileSystemEventDb::ItemStatus::NewAdded) // Remove new added files since they're temporary
            database->deleteFile(currentPath);
        else
            database->setStatusOfFile(currentPath, FileSystemEventDb::ItemStatus::Deleted);

        emit signalEventDbUpdated();
    }
}

void FileMonitoringManager::slotOnModificationEventDetected(const QString &fileName, const QString &dir)
{
    qDebug() << "updateEvent = " << dir << fileName;
    qDebug() << "";

    QString currentPath = QDir::toNativeSeparators(dir + fileName);

    bool isFileMonitored = database->isFileExist(currentPath);
    if(isFileMonitored)
    {
        FileSystemEventDb::ItemStatus status = database->getStatusOfFile(currentPath);

        if(status != FileSystemEventDb::ItemStatus::NewAdded) // Do not count update events on new added file
        {
            QString oldFileName = database->getOldNameOfFile(currentPath);

            if(oldFileName.isEmpty())
                database->setStatusOfFile(currentPath, FileSystemEventDb::ItemStatus::Updated);
            else
                database->setStatusOfFile(currentPath, FileSystemEventDb::ItemStatus::UpdatedAndRenamed);
        }

        emit signalEventDbUpdated();
    }
}

void FileMonitoringManager::slotOnMoveEventDetected(const QString &fileName, const QString &oldFileName, const QString &dir)
{
    qDebug() << "renameEvent (old) -> (new) = " << oldFileName << fileName << dir;
    qDebug() << "";

    QString currentOldPath = QDir::toNativeSeparators(dir + oldFileName);
    QString currentNewPath = QDir::toNativeSeparators(dir + fileName);
    auto fsm = FileStorageManager::instance();
    QFileInfo info(currentNewPath);

    if(info.isFile() && !info.isHidden()) // Only accept real files.
    {
        FileSystemEventDb::ItemStatus currentStatus = database->getStatusOfFile(currentOldPath);

        bool isOldFileMonitored = database->isFileExist(currentOldPath);
        bool isNewFileMonitored = database->isFileExist(currentNewPath);

        if(isOldFileMonitored && !isNewFileMonitored)
        {
            // Do not count renames (moves) if files is new added.
            if(currentStatus != FileSystemEventDb::ItemStatus::NewAdded)
            {
                if(currentStatus == FileSystemEventDb::ItemStatus::Updated ||
                   currentStatus == FileSystemEventDb::UpdatedAndRenamed)
                {
                    database->setStatusOfFile(currentOldPath, FileSystemEventDb::ItemStatus::UpdatedAndRenamed);
                }
                else
                    database->setStatusOfFile(currentOldPath, FileSystemEventDb::ItemStatus::Renamed);
            }

            bool isFilePersists = fsm->getFileJsonByUserPath(currentOldPath)[JsonKeys::IsExist].toBool();
            if(isFilePersists)
                database->setOldNameOfFile(currentOldPath, oldFileName);

            database->setNameOfFile(currentOldPath, fileName);

            emit signalEventDbUpdated();
        }
    }
    else if(info.isDir())
    {
        bool isOldFolderMonitored = database->isFolderExist(currentOldPath);
        bool isNewFolderMonitored = database->isFolderExist(currentNewPath);

        if(isOldFolderMonitored && !isNewFolderMonitored)
        {
            FileSystemEventDb::ItemStatus currentStatus = database->getStatusOfFolder(currentOldPath);

            // Do not count renames (moves) if files is new added.
            if(currentStatus != FileSystemEventDb::ItemStatus::NewAdded)
                database->setStatusOfFolder(currentOldPath, FileSystemEventDb::ItemStatus::Renamed);

            QString userFolderPath = currentOldPath + FileStorageManager::separator;
            bool isFolderPersists = fsm->getFolderJsonByUserPath(userFolderPath)[JsonKeys::IsExist].toBool();
            if(isFolderPersists)
                database->setOldNameOfFolder(currentOldPath, oldFileName);

            database->setPathOfFolder(currentOldPath, currentNewPath);

            emit signalEventDbUpdated();
        }

    }
}
