#include "FileSystemEventDb.h"

#include <QReadLocker>
#include <QWriteLocker>

FileSystemEventDb::FileSystemEventDb()
{
    lock = new QReadWriteLock(QReadWriteLock::RecursionMode::Recursive);
}

FileSystemEventDb::~FileSystemEventDb()
{
    delete lock;
}

bool FileSystemEventDb::isMonitoredFolderExist(const QString &userFolderPath) const
{
    QReadLocker readLocker(lock);
    
    bool result = folderMap.contains(userFolderPath);
    return result;
}

bool FileSystemEventDb::addMonitoredFolder(const QString &userFolderPath, efsw::WatchID watchID)
{
    if(isMonitoredFolderExist(userFolderPath))
        return true;

    QWriteLocker writeLocker(lock);
    
    folderMap.insert(userFolderPath, watchID);
    statusMap.insert(userFolderPath, ItemStatus::Monitored);
    return true;
}

bool FileSystemEventDb::setStatusOfMonitoredFolder(const QString &userFolderPath, ItemStatus status)
{
    if(!isMonitoredFolderExist(userFolderPath))
        return false;

    QWriteLocker writeLocker(lock);

    statusMap.remove(userFolderPath);
    statusMap.insert(userFolderPath, status);

    return true;
}

QHash<FileSystemEventDb::ItemStatus, QStringList> FileSystemEventDb::getEventsOnMonitoredFolders() const
{
    QReadLocker readLocker(lock);

    QStringList deletedFolderList;
    QStringList renamedFolderList;

    QHashIterator<QString, efsw::WatchID> hashIter(folderMap);
    while(hashIter.hasNext())
    {
        hashIter.next();

        QString currentFolderPath = hashIter.key();

        FileSystemEventDb::ItemStatus value = statusMap.value(currentFolderPath, FileSystemEventDb::ItemStatus::Invalid);

        if(value == FileSystemEventDb::ItemStatus::Deleted)
            deletedFolderList.append(currentFolderPath);
        else if(value == FileSystemEventDb::ItemStatus::Renamed)
            renamedFolderList.append(currentFolderPath);
    }

    QHash<FileSystemEventDb::ItemStatus, QStringList> result;
    result.insert(FileSystemEventDb::ItemStatus::Deleted, deletedFolderList);
    result.insert(FileSystemEventDb::ItemStatus::Renamed, renamedFolderList);
    return result;
}

bool FileSystemEventDb::isMonitoredFileExist(const QString &userFolderPath, const QString &fileName) const
{
    QReadLocker readLocker(lock);

    QSet<QString> emptySet;
    QSet<QString> fileSet = fileMap.value(userFolderPath, emptySet);
    if(fileSet.contains(fileName))
        return true;

    return false;
}

bool FileSystemEventDb::addMonitoredFile(const QString &userFolderPath, const QString &fileName)
{
    if(!isMonitoredFolderExist(userFolderPath))
        return false;
    
    if(isMonitoredFileExist(userFolderPath, fileName))
        return true;

    QWriteLocker writeLocker(lock);

    QSet<QString> fileSet = fileMap.value(userFolderPath);
    fileSet.insert(fileName);

    fileMap.insert(userFolderPath, fileSet);
    statusMap.insert(userFolderPath + fileName, ItemStatus::Monitored);

    return true;
}

bool FileSystemEventDb::setStatusOfMonitoredFile(const QString &userFolderPath, const QString &fileName, ItemStatus status)
{
    if(!isMonitoredFileExist(userFolderPath, fileName))
        return false;

    QWriteLocker writeLocker(lock);

    QString key = userFolderPath + fileName;
    statusMap.remove(key);
    statusMap.insert(key, status);

    return true;
}

QHash<FileSystemEventDb::ItemStatus, QStringList> FileSystemEventDb::getEventsOnMonitoredFiles() const
{
    QReadLocker readLocker(lock);

    QStringList updatedFileList;
    QStringList deletedFileList;
    QStringList renamedFileList;

    QHashIterator<QString, QSet<QString>> hashIter(fileMap);
    while(hashIter.hasNext())
    {
        hashIter.next();

        for(const QString &fileName : hashIter.value())
        {
            QString currentFilePath = hashIter.key() + fileName;
            FileSystemEventDb::ItemStatus value = statusMap.value(currentFilePath, FileSystemEventDb::ItemStatus::Invalid);

            if(value == FileSystemEventDb::ItemStatus::Updated)
                updatedFileList.append(currentFilePath);
            else if(value == FileSystemEventDb::ItemStatus::Deleted)
                deletedFileList.append(currentFilePath);
            else if(value == FileSystemEventDb::ItemStatus::Renamed)
                renamedFileList.append(currentFilePath);
        }
    }

    QHash<FileSystemEventDb::ItemStatus, QStringList> result;
    result.insert(FileSystemEventDb::ItemStatus::Updated, updatedFileList);
    result.insert(FileSystemEventDb::ItemStatus::Deleted, deletedFileList);
    result.insert(FileSystemEventDb::ItemStatus::Renamed, renamedFileList);
    return result;
}

QString FileSystemEventDb::getNewPathByOldPath(const QString &oldPath) const
{
    QReadLocker readLocker(lock);
    QString result = forwardRenamingMap.value(oldPath);
    return result;
}

void FileSystemEventDb::addRenamingEntry(const QString &oldPath, const QString &newPath)
{
    QWriteLocker writeLocker(lock);
    
    forwardRenamingMap.insert(oldPath, newPath);
}

QStringList FileSystemEventDb::getForwardRenamingChain(const QString &startPath) const
{
    QReadLocker readLocker(lock);

    QStringList result;
    QString currentEntry = startPath;
    
    while(forwardRenamingMap.contains(currentEntry))
    {
        result.append(currentEntry);;
        QString nextEntry = forwardRenamingMap.value(currentEntry);
        currentEntry = nextEntry;
    }

    return result;
}

QStringList FileSystemEventDb::getbackwardRenamingChain(const QString &startPath) const
{
    QReadLocker readLocker(lock);

    QStringList result;
    QString currentEntry = startPath;

    while(backwardRenamingMap.contains(currentEntry))
    {
        result.append(currentEntry);;
        QString nextEntry = forwardRenamingMap.value(currentEntry);
        currentEntry = nextEntry;
    }

    return result;
}

void FileSystemEventDb::addNewAddedFolder(const QString &userFolderPath, efsw::WatchID watchID)
{
    QWriteLocker writeLocker(lock);

    newFolderMap.insert(userFolderPath, watchID);
}

void FileSystemEventDb::removeNewAddedFolder(const QString &userFolderPath)
{
    QWriteLocker writeLocker(lock);

    newFolderMap.remove(userFolderPath);
}

QHash<QString, efsw::WatchID> FileSystemEventDb::getNewAddedFolderMap() const
{
    QReadLocker readLocker(lock);

    return newFolderMap;
}

void FileSystemEventDb::addNewAddedFile(const QString &userFolderPath, const QString &fileName)
{
    QWriteLocker writeLocker(lock);

    QSet<QString> fileSet = newFileMap.value(userFolderPath);
    fileSet.insert(fileName);

    newFileMap.insert(userFolderPath, fileSet);
}

void FileSystemEventDb::removeNewAddedFile(const QString &userFolderPath, const QString &fileName)
{
    QWriteLocker writeLocker(lock);

    if(newFileMap.contains(userFolderPath))
    {
        QSet<QString> fileSet = newFileMap.value(userFolderPath);
        fileSet.remove(fileName);

        newFileMap.insert(userFolderPath, fileSet);
    }
}

QStringList FileSystemEventDb::getFolderListOfNewAddedFiles() const
{
    QReadLocker readLocker(lock);

    return newFileMap.keys();
}

QSet<QString> FileSystemEventDb::getNewAddedFileSet(const QString &userFolderPath) const
{
    QReadLocker readLocker(lock);

    return newFileMap.value(userFolderPath);
}

QStringList FileSystemEventDb::getNewAddedFileList() const
{
    QStringList result;

    for(const QString &currentFolderPath : getFolderListOfNewAddedFiles())
    {
        QSet<QString> fileSet = getNewAddedFileSet(currentFolderPath);

        for(const QString &currentFileName : fileSet)
            result.append(currentFolderPath + currentFileName);
    }

    return result;
}
