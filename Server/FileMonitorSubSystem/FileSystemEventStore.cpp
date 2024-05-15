#include "FileSystemEventStore.h"

#include <QReadLocker>
#include <QWriteLocker>

QReadWriteLock FileSystemEventStore::folderLock;
QReadWriteLock FileSystemEventStore::fileLock;

FileSystemEventStore::FileSystemEventStore() {}

void FileSystemEventStore::addFolder(const QString &path, Status status)
{
    QWriteLocker writeLocker(&folderLock);

    folderMap.insert(path, status);
}

void FileSystemEventStore::removeFolder(const QString &path)
{
    QWriteLocker writeLocker(&folderLock);

    folderMap.remove(path);
}

FileSystemEventStore::Status FileSystemEventStore::statusOfFolder(const QString &path) const
{
    QReadLocker readLocker(&folderLock);

    return folderMap.value(path, Status::Invalid);
}

QStringList FileSystemEventStore::folderList() const
{
    QReadLocker readLocker(&folderLock);

    return folderMap.keys();
}

void FileSystemEventStore::addFile(const QString &path, Status status)
{
    QWriteLocker writeLocker(&fileLock);

    fileMap.insert(path, status);
}

void FileSystemEventStore::removeFile(const QString &path)
{
    QWriteLocker writeLocker(&fileLock);

    fileMap.remove(path);
}

FileSystemEventStore::Status FileSystemEventStore::statusOfFile(const QString &path) const
{
    QReadLocker readLocker(&fileLock);

    return fileMap.value(path, Status::Invalid);
}

QStringList FileSystemEventStore::fileList() const
{
    QReadLocker readLocker(&fileLock);

    return fileMap.keys();
}

void FileSystemEventStore::clear()
{
    QWriteLocker folderLocker(&folderLock);
    QWriteLocker fileLocker(&fileLock);

    folderMap.clear();
    fileMap.clear();
}
