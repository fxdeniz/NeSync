#ifndef FILESTORAGESERVICE_H
#define FILESTORAGESERVICE_H

#include <QObject>

class FileStorageService : public QObject
{
    Q_OBJECT
public:
    explicit FileStorageService(QObject *parent = nullptr);

    bool deleteFile(const QString& symbolFilePath);

signals:
};

#endif // FILESTORAGESERVICE_H
