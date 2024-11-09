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
    QHttpServerResponse setZipFilePath(const QHttpServerRequest& request);
    QHttpServerResponse getZipFilePath(const QHttpServerRequest& request);
    QHttpServerResponse openArchive(const QHttpServerRequest& request);
    QHttpServerResponse readFoldersJson(const QHttpServerRequest& request);

signals:

private:
    ZipImportService service;
};

#endif // ZIPIMPORTCONTROLLER_H
