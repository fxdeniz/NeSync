#ifndef FILESTORAGESERVICE_H
#define FILESTORAGESERVICE_H

#include <QObject>

class FileStorageService : public QObject
{
    Q_OBJECT
public:
    explicit FileStorageService(QObject *parent = nullptr);

    bool renameFile(const QString &symbolFilePath, const QString &fileName);
    bool deleteFile(const QString &symbolFilePath);

    QString lastSymbolFilePath() const;

signals:

private:
    QString _lastSymbolFilePath;
};

#endif // FILESTORAGESERVICE_H
