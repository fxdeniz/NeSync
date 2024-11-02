#ifndef ZIPEXPORTCONTROLLER_H
#define ZIPEXPORTCONTROLLER_H

#include <QObject>
#include <QJsonObject>
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
    QHttpServerResponse postAddFolderJson(const QHttpServerRequest& request);
    QHttpServerResponse postAddFolderJson_V1(const QHttpServerRequest& request);
    QHttpServerResponse postAddFileJson(const QHttpServerRequest& request);
    QHttpServerResponse postAddFileJson_V1(const QHttpServerRequest& request);
    QHttpServerResponse postAddFileJson_V2(const QHttpServerRequest& request);
    QHttpServerResponse postAddFileToZip(const QHttpServerRequest& request);

    QJsonObject getFilesJson() const;
    void setFilesJson(const QJsonObject &newFileJson);

signals:

private:
    QString zipFilePath;
    QString rootSymbolFolderPath;
    QJsonObject filesJson;

};

#endif // ZIPEXPORTCONTROLLER_H
