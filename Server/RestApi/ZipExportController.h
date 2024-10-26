#ifndef ZIPEXPORTCONTROLLER_H
#define ZIPEXPORTCONTROLLER_H

#include <QObject>
#include <QHttpServerRequest>
#include <QHttpServerResponse>

class ZipExportController : public QObject
{
    Q_OBJECT
public:
    explicit ZipExportController(QObject *parent = nullptr);
    QHttpServerResponse postCreateArchive(const QHttpServerRequest& request);

signals:

};

#endif // ZIPEXPORTCONTROLLER_H
