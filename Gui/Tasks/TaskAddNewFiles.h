#ifndef TASKADDNEWFILES_H
#define TASKADDNEWFILES_H

#include <QObject>
#include <QRunnable>

#include "Backend/FileStorageSubSystem/FileStorageManager.h"

class TaskAddNewFiles : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit TaskAddNewFiles(QObject *parent = nullptr);

    void addFile(const QString &pathToFile);

private:
    FileStorageManager *fsm;
    QSet<QString> fileSet;

signals:


    // QRunnable interface
public:
    void run() override;
};

#endif // TASKADDNEWFILES_H
