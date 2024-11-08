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
    QHttpServerResponse postSetZipFilePath(const QHttpServerRequest& request);
    QHttpServerResponse getZipFilePath(const QHttpServerRequest& request);
    QHttpServerResponse postSetRootSymbolFolderPath(const QHttpServerRequest& request);
    QHttpServerResponse getRootSymbolFolderPath(const QHttpServerRequest& request);
    QHttpServerResponse postCreateArchive(const QHttpServerRequest& request);
    QHttpServerResponse postAddFoldersJson(const QHttpServerRequest& request);
    QHttpServerResponse postAddFileJson(const QHttpServerRequest& request);
    QHttpServerResponse postAddFileToZip(const QHttpServerRequest& request);

signals:

private:
    ZipExportService service;

};

#endif // ZIPEXPORTCONTROLLER_H
