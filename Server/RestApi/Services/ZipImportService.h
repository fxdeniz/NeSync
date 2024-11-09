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
    QJsonObject getFilesJson() const;
    bool openArchive() const;
    QJsonArray readFoldersJson();
    QJsonObject readFilesJson();

signals:

private:
    void setFoldersJson(const QJsonArray &newFoldersJson);
    void setFilesJson(const QJsonObject &newFoldersJson);

    QString zipFilePath;
    QJsonArray foldersJson;
    QJsonObject filesJson;
};

#endif // ZIPIMPORTSERVICE_H
