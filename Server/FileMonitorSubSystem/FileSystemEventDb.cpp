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

QString FileSystemEventDb::getNewPathByOldPath(const QString &oldPath) const
{
    QReadLocker readLocker(lock);
    QString result = renamingMap.value(oldPath);
    return result;
}

void FileSystemEventDb::addRenamingEntry(const QString &oldPath, const QString &newPath)
{
    QWriteLocker writeLocker(lock);

    renamingMap.insert(oldPath, newPath);
}

void FileSystemEventDb::removeRenamingChain(const QString &oldPath)
{
    QWriteLocker writeLocker(lock);

    QString currentEntry = oldPath;

    while(renamingMap.contains(currentEntry))
    {
        QString nextEntry = renamingMap.value(currentEntry);
        renamingMap.remove(currentEntry);
        currentEntry = nextEntry;
    }
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

QStringList FileSystemEventDb::getNewAddedFileFolders() const
{
    QReadLocker readLocker(lock);

    return newFileMap.keys();
}

QSet<QString> FileSystemEventDb::getNewAddedFileSet(const QString &userFolderPath) const
{
    QReadLocker readLocker(lock);

    return newFileMap.value(userFolderPath);
}
