#ifndef TASKNAVIAGATEFILESYSTEM_H
#define TASKNAVIAGATEFILESYSTEM_H


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
};

#endif // TASKNAVIAGATEFILESYSTEM_H
