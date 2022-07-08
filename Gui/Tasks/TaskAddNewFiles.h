#ifndef TASKADDNEWFILES_H
#define TASKADDNEWFILES_H

#include <QObject>
#include <QRunnable>

#include "Backend/FileStorageSubSystem/FileStorageManager.h"

class TaskAddNewFiles : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit TaskAddNewFiles(const QString &targetSymbolFolder, QStringList fileList, QObject *parent = nullptr);

    int fileCount() const;
    const QString &getTargetSymbolFolder() const;
    bool isAllRequestsSuccessful() const;

signals:
    void signalFileAddedSuccessfully(const QString &pathToFile);
    void signalFileAddingFailed(const QString &pathToFile);
    void signalFileProcessed(int fileNumber);

    // QRunnable interface
public:
    void run() override;

private:
    void setIsAllRequestsSuccessful(bool newIsAllRequestsSuccessful);

private:
    QString targetSymbolFolder;
    QStringList fileList;
    bool _isAllRequestsSuccessful;
};

#endif // TASKADDNEWFILES_H
