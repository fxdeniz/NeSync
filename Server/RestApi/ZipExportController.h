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
    QHttpServerResponse postAddFolderJson(const QHttpServerRequest& request);
    QHttpServerResponse postAddFileJson(const QHttpServerRequest& request);
    QHttpServerResponse postAddFileJson_V1(const QHttpServerRequest& request);

    QString getZipFilePath() const;
    void setZipFilePath(const QString &newZipFilePath);

signals:

private:
    QString zipFilePath;

};

#endif // ZIPEXPORTCONTROLLER_H
