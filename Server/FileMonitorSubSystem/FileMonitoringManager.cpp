#include "FileMonitoringManager.h"
#include "JsonDtoFormat.h"

#include <QFileInfo>
#include <QCryptographicHash>

FileMonitoringManager::FileMonitoringManager(FileStorageManager *fsm,
                                             FileSystemEventStore *fses,
                                             QObject *parent)
    : QObject{parent}
{
    this->fsm = fsm;
    this->fses = fses;

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
    QHashIterator<QString, efsw::WatchID> hashIter(watchedFolderMap);

    hashIter.next();

    while(hashIter.hasNext())
    {
        fileWatcher.removeWatch(hashIter.value());
        hashIter.next();
    }

    delete fsm;
    delete fses;
    fsm = nullptr;
    fses = nullptr;
}

void FileMonitoringManager::addFolder(const QString &userFolderPath)
{
    efsw::WatchID watchId = fileWatcher.addWatch(userFolderPath.toStdString(), &fileSystemEventListener, false);

    if(watchId >= efsw::Errors::NoError)
        watchedFolderMap.insert(userFolderPath, watchId);
}

void FileMonitoringManager::slotOnAddEventDetected(const QString &fileOrFolderName, const QString &dir)
{
    //fses->addFolder(dir + fileOrFolderName, FileSystemEventStore::Status::NewAdded);
}

void FileMonitoringManager::slotOnDeleteEventDetected(const QString &fileOrFolderName, const QString &dir)
{
    //fses->addFolder(dir + fileOrFolderName, FileSystemEventStore::Status::Deleted);
}

void FileMonitoringManager::slotOnModificationEventDetected(const QString &fileName, const QString &dir)
{
    QString completePath = dir + fileName;

    QJsonObject fileJson = fsm->getFileJsonByUserPath(completePath);
    bool isFilePersists = fileJson[JsonKeys::IsExist].toBool();

    if(isFilePersists)
    {
        QFileInfo info(completePath);

        QString symbolPath = fileJson[JsonKeys::File::SymbolFilePath].toString();
        qlonglong versionNumber = fileJson[JsonKeys::File::MaxVersionNumber].toInteger();

        QJsonObject versionJson = fsm->getFileVersionJson(symbolPath, versionNumber);

        QDateTime timestamp = QDateTime::fromString(versionJson[JsonKeys::FileVersion::LastModifiedTimestamp].toString());
        QString hash = versionJson[JsonKeys::FileVersion::Hash].toString();

        QFile file(completePath);
        bool isOpen = file.open(QFile::OpenModeFlag::ReadOnly);

        if(!isOpen)
            return;

        QCryptographicHash hasher(QCryptographicHash::Algorithm::Sha3_256);
        hasher.addData(&file);
        QString fileHash = QString(hasher.result().toHex());

        bool isTimestampChanged = (info.lastModified() != timestamp);
        bool isHashChanged = (hash != fileHash);


        if(isTimestampChanged && isHashChanged)
            fses->addFile(completePath, FileSystemEventStore::Status::Updated);
    }
}

void FileMonitoringManager::slotOnMoveEventDetected(const QString &fileOrFolderName, const QString &oldFileOrFolderName, const QString &dir)
{
    //fses->addFolder(dir + fileOrFolderName, FileSystemEventStore::Status::Renamed);
}
