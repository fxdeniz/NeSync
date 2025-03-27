#ifndef FILESTORAGESERVICE_H
#define FILESTORAGESERVICE_H

#include <QObject>

class FileStorageService : public QObject
{
    Q_OBJECT
public:
    explicit FileStorageService(QObject *parent = nullptr);

    bool deleteFolder(const QString &symbolFolderPath);
    bool renameFile(const QString &symbolFilePath, const QString &fileName);
    bool freezeFile(const QString &symbolFilePath, bool isFrozen);
    bool deleteFile(const QString &symbolFilePath);

    QString lastSymbolFolderPath() const;
    QString lastSymbolFilePath() const;

signals:

private:
    QString _lastSymbolFolderPath;
    QString _lastSymbolFilePath;
};

#endif // FILESTORAGESERVICE_H
