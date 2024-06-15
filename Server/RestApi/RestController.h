#ifndef RESTCONTROLLER_H
#define RESTCONTROLLER_H

#include <QObject>
#include <QThread>
#include <QHttpServerRequest>
#include <QHttpServerResponse>

#include "FileMonitorSubSystem/FileSystemEventStore.h"

class RestController : public QObject
{
    Q_OBJECT
public:
    explicit RestController(QObject *parent = nullptr);

    QHttpServerResponse postAddNewFolder(const QHttpServerRequest& request);
    QHttpServerResponse postAddNewFile(const QHttpServerRequest& request);
    QHttpServerResponse postAppendVersion(const QHttpServerRequest& request);
    QHttpServerResponse getFolderContent(const QHttpServerRequest& request);
    QHttpServerResponse startMonitoring(const QHttpServerRequest& request);
    QHttpServerResponse dumpFses(const QHttpServerRequest& request);
    QHttpServerResponse newAddedList(const QHttpServerRequest& request);
    QHttpServerResponse updatedFileList(const QHttpServerRequest& request);

signals:

private:
    QThread *fileMonitorThread;
    FileSystemEventStore *fses;
};

#endif // RESTCONTROLLER_H
