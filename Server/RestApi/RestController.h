#ifndef RESTCONTROLLER_H
#define RESTCONTROLLER_H

#include <QObject>
#include <QHttpServerRequest>
#include <QHttpServerResponse>


class RestController : public QObject
{
    Q_OBJECT
public:
    explicit RestController(QObject *parent = nullptr);

    QHttpServerResponse postAddNewFolder_V1(const QHttpServerRequest& request);
    QHttpServerResponse postAddNewFolder(const QHttpServerRequest& request);
    QHttpServerResponse postAddNewFile_V1(const QHttpServerRequest& request);
    QHttpServerResponse postAddNewFile(const QHttpServerRequest& request);
    QHttpServerResponse postAppendVersion_V1(const QHttpServerRequest& request);
    QHttpServerResponse postAppendVersion(const QHttpServerRequest& request);
    QHttpServerResponse deleteFolder(const QHttpServerRequest& request);
    QHttpServerResponse deleteFile(const QHttpServerRequest& request);
    QHttpServerResponse getFolderContent(const QHttpServerRequest& request);
    QHttpServerResponse getFolderContentByUserPath(const QHttpServerRequest& request);
    QHttpServerResponse getFileContentByUserPath(const QHttpServerRequest& request);
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

#endif // RESTCONTROLLER_H
