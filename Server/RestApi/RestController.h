#ifndef RESTCONTROLLER_H
#define RESTCONTROLLER_H

#include <QObject>
#include <QThread>
#include <QHttpServerRequest>
#include <QHttpServerResponse>


class RestController : public QObject
{
    Q_OBJECT
public:
    explicit RestController(QObject *parent = nullptr);

    QHttpServerResponse postAddNewFolder(const QHttpServerRequest& request);
    QHttpServerResponse postAddNewFile(const QHttpServerRequest& request);
    QHttpServerResponse postAppendVersion(const QHttpServerRequest& request);
    QHttpServerResponse deleteFolder(const QHttpServerRequest& request);
    QHttpServerResponse deleteFile(const QHttpServerRequest& request);
    QHttpServerResponse getFolderContent(const QHttpServerRequest& request);
    QHttpServerResponse getFolderContentByUserPath(const QHttpServerRequest& request);
    QHttpServerResponse getFileContentByUserPath(const QHttpServerRequest& request);
    QHttpServerResponse newAddedList_V1(const QHttpServerRequest& request);
    QHttpServerResponse newAddedList_V2(const QHttpServerRequest& request);
    QHttpServerResponse newAddedList_V3(const QHttpServerRequest& request);
    QHttpServerResponse newAddedList(const QHttpServerRequest& request);
    QHttpServerResponse deletedList(const QHttpServerRequest& request);
    QHttpServerResponse updatedFileList(const QHttpServerRequest& request);

signals:

};

#endif // RESTCONTROLLER_H
