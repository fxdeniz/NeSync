#include "Backend/FileStorageSubSystem/V2_FileStorageManager.h"
#include "TaskNaviagateFileSystem.h"

#include <QThread>

TaskNaviagateFileSystem::TaskNaviagateFileSystem(QObject *parent)
    : QObject{parent}
{
}


void TaskNaviagateFileSystem::slotOnDirContentRequested(const QString &symbolDirPath)
{
    auto fsm = V2_FileStorageManager::instance();
    QJsonObject result = fsm->getFolderJsonBySymbolPath(symbolDirPath, true);
    emit signalDirContentFetched(result);
}
