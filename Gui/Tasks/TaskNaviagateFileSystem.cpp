#include "TaskNaviagateFileSystem.h"

#include <QThread>

TaskNaviagateFileSystem::TaskNaviagateFileSystem(QObject *parent)
    : QObject{parent}
{
    fsm = FileStorageManager::instance();
}


void TaskNaviagateFileSystem::slotOnDirContentRequested(const QString &directory)
{
    qDebug() << "TaskNaviagateFileSystem::slotOnDirContentRequested() in " << QThread::currentThread()->objectName();
    auto result = fsm->getFolderMetaData(directory);
    emit signalDirContentFetched(result);
}
