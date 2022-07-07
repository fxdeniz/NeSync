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

private:
    QString targetSymbolFolder;
    QStringList fileList;

signals:
    void signalFileAddedSuccessfully(const QString &pathToFile);
    void signalFileAddingFailed(const QString &pathToFile);

    // QRunnable interface
public:
    void run() override;
    const QString &getTargetSymbolFolder() const;
};

#endif // TASKADDNEWFILES_H
