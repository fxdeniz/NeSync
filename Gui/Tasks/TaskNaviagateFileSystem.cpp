#include "TaskNaviagateFileSystem.h"

#include <QThread>

TaskNaviagateFileSystem::TaskNaviagateFileSystem(QObject *parent)
    : QObject{parent}
{
    fsm = FileStorageManager::instance();
}


void TaskNaviagateFileSystem::slotOnDirContentRequested(const QString &directory)
{
    auto result = fsm->getFolderMetaData(directory);
    emit signalDirContentFetched(result);
}
