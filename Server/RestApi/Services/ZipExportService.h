#ifndef ZIPEXPORTSERVICE_H
#define ZIPEXPORTSERVICE_H

#include <QObject>
#include <QJsonObject>

class ZipExportService : public QObject
{
    Q_OBJECT
public:
    explicit ZipExportService(QObject *parent = nullptr);

    QString getZipFilePath() const;
    void setZipFilePath(const QString &newZipFilePath);
    QString getRootSymbolFolderPath() const;
    void setRootSymbolFolderPath(const QString &newRootSymbolFolderPath);
    QJsonObject getFilesJson() const;
    void setFilesJson(const QJsonObject &newFilesJson);
    bool createArchive();
    bool addFoldersJson();
    bool addFileJson();
    bool addFileToZip(QString symbolFilePath, qulonglong versionNumber);

signals:

private:
    QString zipFilePath;
    QString rootSymbolFolderPath;
    QJsonObject filesJson;
};

#endif // ZIPEXPORTSERVICE_H
