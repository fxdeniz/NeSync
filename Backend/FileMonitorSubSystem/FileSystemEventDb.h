#ifndef FILESYSTEMEVENTDB_H
#define FILESYSTEMEVENTDB_H

#include "efsw/efsw.hpp"
#include "qdatetime.h"
#include <QSqlDatabase>

class FileSystemEventDb
{
public:
    enum ItemStatus
    {
        Invalid = -1,
        Undefined = 0,
        NewAdded = 1,
        Updated = 2,
        Renamed = 3,
        UpdatedAndRenamed = 4,
        Deleted = 5
    };

    FileSystemEventDb(const QSqlDatabase &eventDb);
    ~FileSystemEventDb();

    bool isFolderExist(const QString &pathToFolder) const;
    bool isFileExist(const QString &pathToFile) const;
    bool addFolder(const QString &pathToFolder);
    bool addFile(const QString &pathToFile);
    bool deleteFolder(const QString &pathToFolder);
    bool deleteFile(const QString &pathToFile);
    bool setStatusOfFolder(const QString &pathToFolder, ItemStatus status);
    bool setStatusOfFile(const QString &pathToFile, ItemStatus status);
    bool setPathOfFolder(const QString &pathToFolder, const QString &newPath);
    bool setNameOfFile(const QString &pathToFile, const QString &newName);
    bool setOldNameOfFolder(const QString &pathToFolder, const QString &oldName);
    bool setOldNameOfFile(const QString &pathToFile, const QString &oldName);
    bool setEfswIDofFolder(const QString &pathToFolder, long id);
    efsw::WatchID getEfswIDofFolder(const QString &pathToFolder) const;
    ItemStatus getStatusOfFolder(const QString &pathToFolder) const;
    ItemStatus getStatusOfFile(const QString &pathToFile) const;
    QString getOldNameOfFolder(const QString &pathToFolder) const;
    QString getOldNameOfFile(const QString &pathToFile) const;
    QStringList getMonitoredFolderPathList() const;
    QStringList getActiveRootFolderList() const;
    QStringList getDirectChildFolderListOfFolder(const QString pathToFolder) const;
    QStringList getChildFileListOfFolder(const QString &pathToFolder) const;
    QStringList getEventfulFileListOfFolder(const QString &pathToFolder) const;
    bool addMonitoringError(const QString &location, const QString &during, qlonglong error);

private:
    QSqlDatabase database;

    QSqlRecord getFolderRow(const QString &pathToFolder) const;
    QSqlRecord getFileRow(const QString &pathToFile) const;
    void createDb();
};

#endif // FILESYSTEMEVENTDB_H
