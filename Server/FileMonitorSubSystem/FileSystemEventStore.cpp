#include "FileSystemEventStore.h"

#include <QReadLocker>
#include <QWriteLocker>

QReadWriteLock FileSystemEventStore::lock;

FileSystemEventStore::FileSystemEventStore() {}

void FileSystemEventStore::addFolder(const QString &path, Status status)
{
    QWriteLocker writeLocker(&lock);

    folderMap.insert(path, status);
}

void FileSystemEventStore::removeFolder(const QString &path)
{
    QWriteLocker writeLocker(&lock);

    folderMap.remove(path);
}

FileSystemEventStore::Status FileSystemEventStore::statusOfFolder(const QString &path) const
{
    QReadLocker readLocker(&lock);

    return folderMap.value(path, Status::Invalid);
}

QStringList FileSystemEventStore::folderList() const
{
    QReadLocker readLocker(&lock);

    return folderMap.keys();
}

void FileSystemEventStore::addFile(const QString &path, Status status)
{
    QWriteLocker writeLocker(&lock);

    fileMap.insert(path, status);
}

void FileSystemEventStore::removeFile(const QString &path)
{
    QWriteLocker writeLocker(&lock);

    fileMap.remove(path);
}

FileSystemEventStore::Status FileSystemEventStore::statusOfFile(const QString &path) const
{
    QReadLocker readLocker(&lock);

    return fileMap.value(path, Status::Invalid);
}

QStringList FileSystemEventStore::fileList() const
{
    QReadLocker readLocker(&lock);

    return fileMap.keys();
}

void FileSystemEventStore::clear()
{
    QWriteLocker writeLocker(&lock);

    folderMap.clear();
    fileMap.clear();
}
