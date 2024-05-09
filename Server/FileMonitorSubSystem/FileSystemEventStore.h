#ifndef FILESYSTEMEVENTSTORE_H
#define FILESYSTEMEVENTSTORE_H

#include <QHash>
#include <QReadWriteLock>


class FileSystemEventStore
{
public:

    enum Status
    {
        Invalid = -1,
        Monitored = 0,
        NewAdded = 1,
        Updated = 2,
        Renamed = 3,
        Deleted = 4,
        Missing = 5
    };

    static QString statusToString(Status status);

    FileSystemEventStore();

    void addFolder(const QString &path, Status status);
    void removeFolder(const QString &path);
    Status statusOfFolder(const QString &path) const;
    QStringList folderList() const;

    void addFile(const QString &path, Status status);
    void removeFile(const QString &path);
    Status statusOfFile(const QString &path) const;
    QStringList fileList() const;

    void clear();

private:
    QHash<QString, Status> folderMap;
    QHash<QString, Status> fileMap;
    static QReadWriteLock lock;
};

#endif // FILESYSTEMEVENTSTORE_H
