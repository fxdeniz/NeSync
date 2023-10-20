#ifndef FILESYSTEMEVENTDB_H
#define FILESYSTEMEVENTDB_H

#include <QSet>
#include <QHash>
#include <QReadWriteLock>

#include <efsw/efsw.hpp>

class FileSystemEventDb
{
public:
    enum ItemStatus
    {
        Invalid = -1,
        Monitored = 0,
        NewAdded = 1,
        Updated = 2,
        Renamed = 3,
        Deleted = 4,
        Missing = 5
    };

    FileSystemEventDb();
    virtual ~FileSystemEventDb();

    bool isMonitoredFolderExist(const QString &userFolderPath) const;
    bool addMonitoredFolder(const QString &userFolderPath, efsw::WatchID watchID);
    bool setStatusOfMonitoredFolder(const QString &userFolderPath, ItemStatus status);

    bool isMonitoredFileExist(const QString &userFolderPath, const QString &fileName) const;
    bool addMonitoredFile(const QString &userFolderPath, const QString &fileName);
    bool setStatusOfMonitoredFile(const QString &userFolderPath, const QString &fileName, ItemStatus status);

    QString getNewPathByOldPath(const QString &oldPath) const;
    void addRenamingEntry(const QString &oldPath, const QString &newPath);
    void removeRenamingChain(const QString &oldPath);

    void addNewAddedFolder(const QString &userFolderPath);
    void removeNewAddedFolder(const QString &userFolderPath);
    QSet<QString> getNewAddedFolderSet() const;

private:
    QReadWriteLock *lock = nullptr;

    QHash<QString, efsw::WatchID> folderMap;
    QHash<QString, QSet<QString>> fileMap;
    QHash<QString, ItemStatus> statusMap;
    QHash<QString, QString> renamingMap; // Old Path -> New Path

    QSet<QString> newFolderSet;
};

#endif // FILESYSTEMEVENTDB_H
