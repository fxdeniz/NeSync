#include "Backend/FileStorageSubSystem/FileStorageManager.h"
#include "TaskNaviagateFileSystem.h"

#include <QThread>

TaskNaviagateFileSystem::TaskNaviagateFileSystem(QObject *parent)
    : QObject{parent}
{
}


void TaskNaviagateFileSystem::slotOnDirContentRequested(const QString &directory)
{
    auto result = FileStorageManager::instance()->getFolderMetaData(directory);
    emit signalDirContentFetched(result);
}
