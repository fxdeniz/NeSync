#ifndef ZIPIMPORTSERVICE_H
#define ZIPIMPORTSERVICE_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>

class ZipImportService : public QObject
{
    Q_OBJECT
public:
    explicit ZipImportService(QObject *parent = nullptr);

    QString getZipFilePath() const;
    void setZipFilePath(QString newZipFilePath);
    QJsonArray getFoldersJson() const;
    void setFoldersJson(const QJsonArray &newFoldersJson);
    QJsonObject getFilesJson() const;
    void setFilesJson(const QJsonObject &newFoldersJson);
    bool openArchive() const;
    QJsonArray readFoldersJson();

signals:

private:
    QString zipFilePath;
    QJsonArray foldersJson;
    QJsonObject filesJson;
};

#endif // ZIPIMPORTSERVICE_H
