#ifndef TASKNAVIAGATEFILESYSTEM_H
#define TASKNAVIAGATEFILESYSTEM_H

#include "Backend/FileStorageSubSystem/FileStorageManager.h"

#include <QObject>

class TaskNaviagateFileSystem : public QObject
{
    Q_OBJECT
public:
    explicit TaskNaviagateFileSystem(QObject *parent = nullptr);
    virtual ~TaskNaviagateFileSystem();

public slots:
    void onDirContentRequested(const QString &directory);

signals:
    void dirContentFetched(FolderMetaData data);

private:
    FileStorageManager *fsm;
};

#endif // TASKNAVIAGATEFILESYSTEM_H
