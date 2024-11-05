#ifndef FILESYSTEMMONITORCONTROLLER_H
#define FILESYSTEMMONITORCONTROLLER_H

#include <QObject>
#include <QHttpServerRequest>
#include <QHttpServerResponse>

class FileSystemMonitorController : public QObject
{
    Q_OBJECT
public:
    explicit FileSystemMonitorController(QObject *parent = nullptr);
    QHttpServerResponse newAddedList(const QHttpServerRequest& request);
    QHttpServerResponse deletedList(const QHttpServerRequest& request);
    QHttpServerResponse updatedFileList(const QHttpServerRequest& request);

signals:

private:
    QStringList generateRootFoldersList();
    QJsonObject generateChildFolderSuffixObject(QStringList rootFolderList);
    QJsonObject generateRootOfRootFoldersObject(QStringList rootFolderList);
    QJsonObject generateFilesObject(QStringList rootFolderList);
    QStringList generateFoldersList(QStringList rootFolderList);
    QStringList findNewFolders(const QString &rootPath, bool isRecursive = false);
    QStringList findNewFiles(const QString &rootPath, bool isRecursive = false);
};

#endif // FILESYSTEMMONITORCONTROLLER_H
