#ifndef TASKADDNEWFILES_H
#define TASKADDNEWFILES_H

#include <QObject>
#include <QRunnable>

#include "Backend/FileStorageSubSystem/FileStorageManager.h"

class TaskAddNewFiles : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit TaskAddNewFiles(const QString &targetSymbolFolder, QObject *parent = nullptr);

    void addFile(const QString &pathToFile);
    int fileCount() const;

private:
    QString targetSymbolFolder;
    FileStorageManager *fsm;
    QSet<QString> fileSet;

signals:
    void signalFileAddedSuccessfully(const QString &pathToFile);
    void signalFileAddingFailed(const QString &pathToFile);

    // QRunnable interface
public:
    void run() override;
    const QString &getTargetSymbolFolder() const;
};

#endif // TASKADDNEWFILES_H
