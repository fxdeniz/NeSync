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
    QString targetSymbolFolder;
    FileStorageManager *fsm;
    QSet<QString> fileSet;

signals:


    // QRunnable interface
public:
    void run() override;
    const QString &getTargetSymbolFolder() const;
    void setTargetSymbolFolder(const QString &newTargetSymbolFolder);
};

#endif // TASKADDNEWFILES_H
