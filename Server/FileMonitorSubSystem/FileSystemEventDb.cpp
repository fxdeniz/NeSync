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

    bool result = folderDB.contains(userFolderPath);
    return result;
}

bool FileSystemEventDb::addMonitoredFolder(const QString &userFolderPath, efsw::WatchID watchID)
{
    if(isMonitoredFolderExist(userFolderPath))
        return false;

    QWriteLocker writeLocker(lock);

    folderDB.insert(userFolderPath, watchID);
    return true;
}

bool FileSystemEventDb::isMonitoredFileExist(const QString &userFolderPath, const QString &fileName) const
{
    QReadLocker readLocker(lock);

    QSet<QString> emptySet;
    QSet<QString> fileSet = fileDB.value(userFolderPath, emptySet);
    if(fileSet.contains(fileName))
        return true;

    return false;
}

bool FileSystemEventDb::addMonitoredFile(const QString &userFolderPath, const QString &fileName)
{
    if(!isMonitoredFolderExist(userFolderPath))
        return false;
    
    if(isMonitoredFileExist(userFolderPath, fileName))
        return false;

    QWriteLocker writeLocker(lock);

    QSet<QString> fileSet = fileDB.value(userFolderPath);
    fileSet.insert(fileName);

    fileDB.insert(userFolderPath, fileSet);
    return true;
}
