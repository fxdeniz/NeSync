#ifndef RESTCONTROLLER_H
#define RESTCONTROLLER_H

#include <QObject>
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
    QHttpServerResponse getFolderContent(const QHttpServerRequest& request);

signals:
};

#endif // RESTCONTROLLER_H
