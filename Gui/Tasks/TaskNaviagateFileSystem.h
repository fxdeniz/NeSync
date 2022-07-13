#ifndef TASKNAVIAGATEFILESYSTEM_H
#define TASKNAVIAGATEFILESYSTEM_H

#include "Backend/FileStorageSubSystem/FileStorageManager.h"

#include <QObject>

class TaskNaviagateFileSystem : public QObject
{
    Q_OBJECT
public:
    explicit TaskNaviagateFileSystem(QObject *parent = nullptr);

public slots:
    void slotOnDirContentRequested(const QString &directory);

signals:
    void signalDirContentFetched(FolderRequestResult data);

private:
    QSharedPointer<FileStorageManager> fsm;
};

#endif // TASKNAVIAGATEFILESYSTEM_H
