#ifndef FILESYSTEMMONITORCONTROLLER_H
#define FILESYSTEMMONITORCONTROLLER_H

#include "Services/FileSystemMonitorService.h"

#include <QObject>
#include <QHttpServerRequest>
#include <QHttpServerResponse>

class FileSystemMonitorController : public QObject
{
    Q_OBJECT
public:
    explicit FileSystemMonitorController(QObject *parent = nullptr);
    QHttpServerResponse newAddedItems(const QHttpServerRequest& request);
    QHttpServerResponse deletedItems(const QHttpServerRequest& request);
    QHttpServerResponse updatedFiles(const QHttpServerRequest& request);

signals:

private:
    FileSystemMonitorService service;
};

#endif // FILESYSTEMMONITORCONTROLLER_H
