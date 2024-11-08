#ifndef FILESTORAGECONTROLLER_H
#define FILESTORAGECONTROLLER_H

#include <QObject>
#include <QHttpServerRequest>
#include <QHttpServerResponse>


class FileStorageController : public QObject
{
    Q_OBJECT
public:
    explicit FileStorageController(QObject *parent = nullptr);
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

signals:

};

#endif // FILESTORAGECONTROLLER_H
