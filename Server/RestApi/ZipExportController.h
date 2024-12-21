#ifndef ZIPEXPORTCONTROLLER_H
#define ZIPEXPORTCONTROLLER_H

#include "Services/ZipExportService.h"

#include <QObject>
#include <QHttpServerRequest>
#include <QHttpServerResponse>

class ZipExportController : public QObject
{
    Q_OBJECT
public:
    explicit ZipExportController(QObject *parent = nullptr);
    QHttpServerResponse setFilePath(const QHttpServerRequest& request);
    QHttpServerResponse getFilePath(const QHttpServerRequest& request);
    QHttpServerResponse setRootFolder(const QHttpServerRequest& request);
    QHttpServerResponse getRootFolder(const QHttpServerRequest& request);
    QHttpServerResponse createZip(const QHttpServerRequest& request);
    QHttpServerResponse addFoldersJson(const QHttpServerRequest& request);
    QHttpServerResponse addFilesJson(const QHttpServerRequest& request);
    QHttpServerResponse addFile(const QHttpServerRequest& request);

signals:

private:
    ZipExportService service;

};

#endif // ZIPEXPORTCONTROLLER_H
