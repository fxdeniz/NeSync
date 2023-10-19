#ifndef FILESYSTEMEVENTDB_H
#define FILESYSTEMEVENTDB_H

#include <QSet>
#include <QHash>
#include <QReadWriteLock>

#include <efsw/efsw.hpp>

class FileSystemEventDb
{
public:
    FileSystemEventDb();
    virtual ~FileSystemEventDb();

    bool isMonitoredFolderExist(const QString &userFolderPath) const;
    bool addMonitoredFolder(const QString &userFolderPath, efsw::WatchID watchID);

    bool isMonitoredFileExist(const QString &userFolderPath, const QString &fileName) const;
    bool addMonitoredFile(const QString &userFolderPath, const QString &fileName);

private:
    QReadWriteLock *lock = nullptr;

    QHash<QString, efsw::WatchID> folderDB;
    QHash<QString, QSet<QString>> fileDB;
};

#endif // FILESYSTEMEVENTDB_H
