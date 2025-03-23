#ifndef FILESTORAGECONTROLLER_H
#define FILESTORAGECONTROLLER_H

#include "Services/FileStorageService.h"

#include <QObject>
#include <QHttpServerRequest>
#include <QHttpServerResponse>


class FileStorageController : public QObject
{
    Q_OBJECT
public:
    explicit FileStorageController(QObject *parent = nullptr);
    QHttpServerResponse addNewFolder(const QHttpServerRequest& request);
    QHttpServerResponse addNewFile(const QHttpServerRequest& request);
    QHttpServerResponse appendVersion(const QHttpServerRequest& request);
    QHttpServerResponse deleteFolder(const QHttpServerRequest& request);
    QHttpServerResponse renameFile(const QHttpServerRequest& request);
    QHttpServerResponse deleteFile(const QHttpServerRequest& request);
    QHttpServerResponse getFolder(const QHttpServerRequest& request);
    QHttpServerResponse getFolderUserPath(const QHttpServerRequest& request);
    QHttpServerResponse getStorageFolderPath(const QHttpServerRequest& request);
    QHttpServerResponse getFile(const QHttpServerRequest& request);
    QHttpServerResponse getFileByUserPath(const QHttpServerRequest& request);
    QHttpServerResponse updateFileFrozenStatus(const QHttpServerRequest& request);
    QHttpServerResponse updateFileVersionDescription(const QHttpServerRequest& request);

private:
    FileStorageService service;

signals:

};

#endif // FILESTORAGECONTROLLER_H
