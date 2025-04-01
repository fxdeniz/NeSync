#ifndef FILESTORAGECONTROLLER_H
#define FILESTORAGECONTROLLER_H

#include "Services/FileStorageService.h"

#include <QObject>
#include <QHttpServerRequest>
#include <QHttpServerResponse>


// TODO: This class still accesses FileStorageManager instances directly without the FileStorageService.
//       These types of codes must be refactored to go through FileStorageService.
// TODO: When doing the refactor above, file copying operations must be done before the db operationns.
//       Currently, opposite of this happnens and when file operations fails,
//          db is updated with missing file info.
class FileStorageController : public QObject
{
    Q_OBJECT
public:
    explicit FileStorageController(QObject *parent = nullptr);
    QHttpServerResponse addNewFolder(const QHttpServerRequest& request);
    QHttpServerResponse addNewFile(const QHttpServerRequest& request);
    QHttpServerResponse appendVersion(const QHttpServerRequest& request);
    QHttpServerResponse deleteFolder(const QHttpServerRequest& request);
    QHttpServerResponse deleteFile(const QHttpServerRequest& request);
    QHttpServerResponse renameFolder(const QHttpServerRequest& request);
    QHttpServerResponse renameFile(const QHttpServerRequest& request);
    QHttpServerResponse getFolder(const QHttpServerRequest& request);
    QHttpServerResponse getFolderUserPath(const QHttpServerRequest& request);
    QHttpServerResponse getStorageFolderPath(const QHttpServerRequest& request);
    QHttpServerResponse getFile(const QHttpServerRequest& request);
    QHttpServerResponse getFileByUserPath(const QHttpServerRequest& request);
    QHttpServerResponse freezeFolder(const QHttpServerRequest& request);
    QHttpServerResponse freezeFile(const QHttpServerRequest& request);
    QHttpServerResponse relocateFolder(const QHttpServerRequest& request);
    QHttpServerResponse relocateFile(const QHttpServerRequest& request);
    QHttpServerResponse updateFileVersionDescription(const QHttpServerRequest& request);

private:
    FileStorageService service;

signals:

};

#endif // FILESTORAGECONTROLLER_H
