#include "Backend/FileStorageSubSystem/FileStorageManager.h"
#include "TaskNaviagateFileSystem.h"

#include <QThread>

TaskNaviagateFileSystem::TaskNaviagateFileSystem(QObject *parent)
    : QObject{parent}
{
}


void TaskNaviagateFileSystem::slotOnFolderContentRequested(const QString &symbolDirPath)
{
    auto fsm = FileStorageManager::instance();
    QJsonObject result = fsm->getFolderJsonBySymbolPath(symbolDirPath, true);
    emit signalFolderContentFetched(result);
}
