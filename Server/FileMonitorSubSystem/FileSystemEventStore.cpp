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

FileSystemEventStore::Status FileSystemEventStore::statusOfFolder(const QString &folderPath) const
{
    QReadLocker readLocker(&lock);

    return folderMap.value(folderPath, Status::Invalid);
}

QStringList FileSystemEventStore::folderList() const
{
    QReadLocker readLocker(&lock);

    return folderMap.keys();
}

void FileSystemEventStore::clear()
{
    QWriteLocker writeLocker(&lock);

    folderMap.clear();
}
