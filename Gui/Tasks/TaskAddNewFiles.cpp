#include "TaskAddNewFiles.h"

#include <QDir>
#include <QStandardPaths>

TaskAddNewFiles::TaskAddNewFiles(const QString &targetSymbolFolder, QStringList fileList, QObject *parent)
    : QObject{parent}
{
    this->targetSymbolFolder = targetSymbolFolder;
    this->fileList = fileList;
    this->setIsAllRequestsSuccessful(false);
}

int TaskAddNewFiles::fileCount() const
{
    return this->fileList.size();
}

const QString &TaskAddNewFiles::getTargetSymbolFolder() const
{
    return targetSymbolFolder;
}

bool TaskAddNewFiles::isAllRequestsSuccessful() const
{
    return _isAllRequestsSuccessful;
}

void TaskAddNewFiles::setIsAllRequestsSuccessful(bool newIsAllRequestsSuccessful)
{
    _isAllRequestsSuccessful = newIsAllRequestsSuccessful;
}

void TaskAddNewFiles::run()
{
    auto fsm = FileStorageManager::instance();
    int fileNumber = 1;
    bool local_isAllRequestSuccessful = true;

    for(const QString &currentFilePath : qAsConst(fileList))
    {
        QFileInfo fileInfo(currentFilePath);
        QString userDirectory = QDir::toNativeSeparators(fileInfo.absolutePath()) + QDir::separator();

        bool requestResult = fsm->addNewFile(currentFilePath,
                                             this->getTargetSymbolFolder(),
                                             false,
                                             true,
                                             userDirectory);

        if(requestResult == true)
            emit signalFileAddedSuccessfully(currentFilePath);
        else
        {
            local_isAllRequestSuccessful = false;
            emit signalFileAddingFailed(currentFilePath);
        }

        emit signalFileProcessed(fileNumber);
        ++fileNumber;
    }

    if(local_isAllRequestSuccessful == false)
        this->setIsAllRequestsSuccessful(false);
    else
        this->setIsAllRequestsSuccessful(true);
}
