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
    QHttpServerResponse postCreateArchive(const QHttpServerRequest& request);
    QHttpServerResponse postAddFolderJson(const QHttpServerRequest& request);
    QHttpServerResponse postAddFolderJson_V1(const QHttpServerRequest& request);
    QHttpServerResponse postAddFileJson(const QHttpServerRequest& request);
    QHttpServerResponse postAddFileJson_V1(const QHttpServerRequest& request);
    QHttpServerResponse postAddFileJson_V2(const QHttpServerRequest& request);
    QHttpServerResponse postAddFileToZip(const QHttpServerRequest& request);


    QString getZipFilePath() const;
    void setZipFilePath(const QString &newZipFilePath);
    QJsonObject getFilesJson() const;
    void setFilesJson(const QJsonObject &newFileJson);

signals:

private:
    QString zipFilePath;
    QJsonObject filesJson;

};

#endif // ZIPEXPORTCONTROLLER_H
