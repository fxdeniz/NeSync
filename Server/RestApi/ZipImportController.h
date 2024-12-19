#ifndef ZIPIMPORTCONTROLLER_H
#define ZIPIMPORTCONTROLLER_H

#include "Services/ZipImportService.h"

#include <QObject>
#include <QHttpServerResponse>

class ZipImportController : public QObject
{
    Q_OBJECT
public:
    explicit ZipImportController(QObject *parent = nullptr);
    QHttpServerResponse setFilePath(const QHttpServerRequest& request);
    QHttpServerResponse getFilePath(const QHttpServerRequest& request);
    QHttpServerResponse openZip(const QHttpServerRequest& request);
    QHttpServerResponse readFoldersJson(const QHttpServerRequest& request);
    QHttpServerResponse readFilesJson(const QHttpServerRequest& request);
    QHttpServerResponse importFileFromZip(const QHttpServerRequest& request);

signals:

private:
    ZipImportService service;
};

#endif // ZIPIMPORTCONTROLLER_H
