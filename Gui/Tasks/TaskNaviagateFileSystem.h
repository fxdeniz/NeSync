#ifndef TASKNAVIAGATEFILESYSTEM_H
#define TASKNAVIAGATEFILESYSTEM_H

#include <QObject>
#include <QJsonObject>

class TaskNaviagateFileSystem : public QObject
{
    Q_OBJECT
public:
    explicit TaskNaviagateFileSystem(QObject *parent = nullptr);

public slots:
    void slotOnFolderContentRequested(const QString &symbolDirPath);

signals:
    void signalFolderContentFetched(QJsonObject data);
};

#endif // TASKNAVIAGATEFILESYSTEM_H
