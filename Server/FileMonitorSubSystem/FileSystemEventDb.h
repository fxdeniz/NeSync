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

    QHash<FileSystemEventDb::ItemStatus, QStringList> getEventsOnMonitoredFolders() const;

    bool isMonitoredFileExist(const QString &userFolderPath, const QString &fileName) const;
    bool addMonitoredFile(const QString &userFolderPath, const QString &fileName);
    bool setStatusOfMonitoredFile(const QString &userFolderPath, const QString &fileName, ItemStatus status);

    QHash<FileSystemEventDb::ItemStatus, QStringList> getEventsOnMonitoredFiles() const;

    void addFolderRenamingEntry(const QString &oldUserFolderPath, const QString &newUserFolderPath);
    QString getOriginalFolderNameByNewName(const QString &userFolderPath) const;
    void removeFolderRenamingEntry(const QString &newUserFolderPath);

    void addFileRenamingEntry(const QString &userFolderPath, const QString &oldFileName, const QString &newFileName);
    QString getOriginalFileNameByNewName(const QString &userFolderPath, const QString &fileName) const;
    void removeFileRenamingEntry(const QString &userFolderPath, const QString &newFileName);

    void addNewAddedFolder(const QString &userFolderPath, efsw::WatchID watchID);
    void removeNewAddedFolder(const QString &userFolderPath);
    QHash<QString, efsw::WatchID> getNewAddedFolderMap() const;

    void addNewAddedFile(const QString &userFolderPath, const QString &fileName);
    void removeNewAddedFile(const QString &userFolderPath, const QString &fileName);
    QStringList getFolderListOfNewAddedFiles() const;
    QSet<QString> getNewAddedFileSet(const QString &userFolderPath) const;
    QStringList getNewAddedFileList() const;

private:
    QReadWriteLock *lock = nullptr;

    QHash<QString, efsw::WatchID> folderMap;
    QHash<QString, QSet<QString>> fileMap;
    QHash<QString, ItemStatus> statusMap;
    QHash<QString, QString> newFolderNameToOldMap;
    QHash<QString, QString> newFileNameToOldMap;

    QHash<QString, efsw::WatchID> newFolderMap;
    QHash<QString, QSet<QString>> newFileMap;
};

#endif // FILESYSTEMEVENTDB_H
