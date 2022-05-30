#ifndef MONITOREDDIRDB_H
#define MONITOREDDIRDB_H

#include "Backend/FileMonitorSubSystem/efsw/efsw.hpp"

#include <QSqlDatabase>
#include <QDateTime>

class MonitoredDirDb
{
public:
    typedef enum
    {
        InValid = -1,
        Monitored = 0,
        NewAdded = 1,
        Deleted = 2,
        Modified = 3,
        Moved = 4,
        MovedAndModified = 5

    } MonitoredItemState;

public:
    explicit MonitoredDirDb();

    bool isDirExistByPath(const QString &dir) const;
    bool isDirExistByEfswWatchID(efsw::WatchID id) const;
    bool isFileExistInDir(const QString &fileName, const QString &dir) const;
    bool addDir(const QString &dir, efsw::WatchID id);
    bool removeDir(const QString &dir);
    bool renameDir(const QString &oldDir, const QString &newDir);
    bool addFileToDir(const QString &fileName, const QString &dir);
    bool removeFileFromDir(const QString &fileName, const QString &dir);
    bool renameFileInDir(const QString &oldFileName, const QString &newFileName, const QString &dir);
    bool updateOldFileNameOfFileInDir(const QString &fileName, const QString &oldFileName, const QString &dir);
    QString oldFileNameOfFileInDir(const QString &fileName, const QString &dir) const;
    bool updateOldNameOfDir(const QString &oldDirName, const QString &dir);
    QString oldNameOfDir(const QString &dir) const;
    bool moveFileBetweenDirs(const QString &fileName, const QString &fromDir, const QString &toDir);
    bool updateEfswWatchIDofDir(const QString &dir, efsw::WatchID newID);
    qlonglong efswWatchIDofDir(const QString &dir) const;
    QList<qlonglong> efswWatchIDList() const;
    int fileCountInDir(const QString &dir) const;
    QStringList fileListInDir(const QString &dir) const;
    bool updateEventTimestampOfDir(const QString &dir, const QDateTime &newTimestamp);
    QDateTime eventTimestampOfDir(const QString &dir) const;
    bool updateEventTimestampOfFileInDir(const QString &fileName, const QString &dir, const QDateTime &newTimestamp);
    QDateTime eventTimestampOfFileInDir(const QString &fileName, const QString &dir) const;
    bool scheduleDirAs(const QString &dir, MonitoredItemState state);
    QStringList scheduledDirList(MonitoredItemState criteria) const;
    bool unScheduleDir(const QString &dir);
    QStringList scheduledFileList(MonitoredItemState criteria) const;
    bool scheduleFileInDirAs(const QString &fileName, const QString &dir, MonitoredItemState state);
    bool unScheduleFileInDir(const QString &fileName, const QString &dir);
    MonitoredItemState stateOfDir(const QString &dir) const;
    MonitoredItemState stateOfFileInDir(const QString &fileName, const QString &dir) const;
    bool hasContainScheduledFiles() const;
    bool resetDb();

    QString standardizeDir(const QString &dir) const;

private:
    void createDatabase();
    bool dropAllTables();


private:
    QSqlDatabase db;
};

#endif // MONITOREDDIRDB_H
