#include "TaskNaviagateFileSystem.h"

#include <QThread>

TaskNaviagateFileSystem::TaskNaviagateFileSystem(QObject *parent)
    : QObject{parent}
{
    fsm = FileStorageManager::instance().data();
}

TaskNaviagateFileSystem::~TaskNaviagateFileSystem()
{
    delete fsm;
}

void TaskNaviagateFileSystem::onDirContentRequested(const QString &directory)
{
    qDebug() << "TaskNaviagateFileSystem::onDirContentRequested() in " << QThread::currentThread()->objectName();
    auto result = fsm->getFolderMetaData(directory);
    emit dirContentFetched(result);
}
