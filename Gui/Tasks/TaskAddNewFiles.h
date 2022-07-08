#ifndef TASKADDNEWFILES_H
#define TASKADDNEWFILES_H

#include <QThread>

#include "Backend/FileStorageSubSystem/FileStorageManager.h"

class TaskAddNewFiles : public QThread
{
    Q_OBJECT
public:
    explicit TaskAddNewFiles(const QString &targetSymbolFolder, QStringList fileList, QObject *parent = nullptr);
    virtual ~TaskAddNewFiles();

    int fileCount() const;
    const QString &getTargetSymbolFolder() const;

signals:
    void signalFileAddedSuccessfully(const QString &pathToFile);
    void signalFileAddingFailed(const QString &pathToFile);
    void signalFileProcessed(int fileNumber);
    void finished(bool isAllRequestsSuccessful); // Overloaded QThread::finished()

    // QThread interface
public:
    void run() override;

private:
    QString targetSymbolFolder;
    QStringList fileList;
};

#endif // TASKADDNEWFILES_H
