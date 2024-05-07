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

    QString completePath = dir + fileOrFolderName;

    QJsonObject fileJson = fsm->getFileJsonByUserPath(completePath);
    QJsonObject folderJson = fsm->getFolderJsonByUserPath(completePath);
    bool isFilePersists = fileJson[JsonKeys::IsExist].toBool();
    bool isFolderPersists = folderJson[JsonKeys::IsExist].toBool();

    if(isFilePersists)
    {
        QString symbolPath = fileJson[JsonKeys::File::SymbolFilePath].toString();
        qlonglong versionNumber = fileJson[JsonKeys::File::MaxVersionNumber].toInteger();

        QJsonObject versionJson = fsm->getFileVersionJson(symbolPath, versionNumber);

        QString hash = versionJson[JsonKeys::FileVersion::Hash].toString();

        QFile file(completePath);
        bool isOpen = file.open(QFile::OpenModeFlag::ReadOnly);

        if(!isOpen) // TODO: Add here more preliminary checks like file.exist(), isFile() etc...
            return;

        QCryptographicHash hasher(QCryptographicHash::Algorithm::Sha3_256);
        hasher.addData(&file);
        QString fileHash = QString(hasher.result().toHex());

        bool isHashChanged = (hash != fileHash);

        if(isHashChanged)
            fses->addFile(completePath, FileSystemEventStore::Status::Updated);

        return;
    }
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
        QString symbolPath = fileJson[JsonKeys::File::SymbolFilePath].toString();
        qlonglong versionNumber = fileJson[JsonKeys::File::MaxVersionNumber].toInteger();

        QJsonObject versionJson = fsm->getFileVersionJson(symbolPath, versionNumber);

        QString hash = versionJson[JsonKeys::FileVersion::Hash].toString();

        QFile file(completePath);
        bool isOpen = file.open(QFile::OpenModeFlag::ReadOnly);

        if(!isOpen) // TODO: Add here more preliminary checks like file.exist(), isFile() etc...
            return;

        QCryptographicHash hasher(QCryptographicHash::Algorithm::Sha3_256);
        hasher.addData(&file);
        QString fileHash = QString(hasher.result().toHex());

        bool isHashChanged = (hash != fileHash);

        if(isHashChanged)
            fses->addFile(completePath, FileSystemEventStore::Status::Updated);
    }
}

void FileMonitoringManager::slotOnMoveEventDetected(const QString &fileOrFolderName, const QString &oldFileOrFolderName, const QString &dir)
{
    //fses->addFolder(dir + fileOrFolderName, FileSystemEventStore::Status::Renamed);
}
