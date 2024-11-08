#ifndef FILESYSTEMMONITORSERVICE_H
#define FILESYSTEMMONITORSERVICE_H

#include <QObject>

class FileSystemMonitorService : public QObject
{
    Q_OBJECT
public:
    explicit FileSystemMonitorService(QObject *parent = nullptr);
    QJsonObject deletedItemsObject() const;
    QJsonObject updatedFilesObject() const;

    QStringList generateRootFoldersList() const;
    QJsonObject generateChildFolderSuffixObject(QStringList rootFolderList) const;
    QJsonObject generateRootOfRootFoldersObject(QStringList rootFolderList) const;
    QJsonObject generateFilesObject(QStringList rootFolderList) const;
    QStringList generateFoldersList(QStringList rootFolderList) const;
    QStringList findNewFolders(QString rootPath, bool isRecursive = false) const;
    QStringList findNewFiles(QString rootPath, bool isRecursive = false) const;

signals:
};

#endif // FILESYSTEMMONITORSERVICE_H
