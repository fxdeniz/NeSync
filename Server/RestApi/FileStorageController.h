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
    QHttpServerResponse addNewFolder(const QHttpServerRequest& request);
    QHttpServerResponse postAddNewFile_V1(const QHttpServerRequest& request);
    QHttpServerResponse addNewFile(const QHttpServerRequest& request);
    QHttpServerResponse postAppendVersion_V1(const QHttpServerRequest& request);
    QHttpServerResponse appendVersion(const QHttpServerRequest& request);
    QHttpServerResponse deleteFolder(const QHttpServerRequest& request);
    QHttpServerResponse deleteFile(const QHttpServerRequest& request);
    QHttpServerResponse getFolder(const QHttpServerRequest& request);
    QHttpServerResponse getFolderUserPath(const QHttpServerRequest& request);
    QHttpServerResponse getFile(const QHttpServerRequest& request);
    QHttpServerResponse getFileByUserPath(const QHttpServerRequest& request);

signals:

};

#endif // FILESTORAGECONTROLLER_H
