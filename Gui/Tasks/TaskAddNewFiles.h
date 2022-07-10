#ifndef TASKADDNEWFILES_H
#define TASKADDNEWFILES_H

#include <QThread>

#include "Backend/FileStorageSubSystem/FileStorageManager.h"

class TaskAddNewFiles : public QThread
{
    Q_OBJECT
public:
    explicit TaskAddNewFiles(const QString &targetSymbolFolder, QObject *parent = nullptr);
    virtual ~TaskAddNewFiles();

    void addAutoSyncEnabled(const QString &pathToFile);
    void addAutoSyncDisabled(const QString &pathToFile);

    int fileCount() const;
    const QString &getTargetSymbolFolder() const;

signals:
    void signalFileBeingProcessed(const QString &pathToFile);
    void signalFileAddedSuccessfully(const QString &pathToFile);
    void signalFileAddingFailed(const QString &pathToFile);
    void signalFileProcessed(int fileNumber);
    void finished(bool isAllRequestsSuccessful); // Overloaded QThread::finished()

    // QThread interface
public:
    void run() override;

private:
    QString targetSymbolFolder;
    QHash<QString, bool> fileMap;
};

#endif // TASKADDNEWFILES_H
