#include "TaskAddNewFiles.h"

#include <QDir>
#include <QStandardPaths>

TaskAddNewFiles::TaskAddNewFiles(const QString &targetSymbolFolder, QStringList fileList, QObject *parent)
    : QThread{parent}
{
    this->targetSymbolFolder = targetSymbolFolder;
    this->fileList = fileList;
}

TaskAddNewFiles::~TaskAddNewFiles()
{
}

int TaskAddNewFiles::fileCount() const
{
    return this->fileList.size();
}

const QString &TaskAddNewFiles::getTargetSymbolFolder() const
{
    return targetSymbolFolder;
}

void TaskAddNewFiles::run()
{
    auto fsm = FileStorageManager::instance();
    int fileNumber = 1;
    bool isAllRequestSuccessful = true;

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
            isAllRequestSuccessful = false;
            emit signalFileAddingFailed(currentFilePath);
        }

        emit signalFileProcessed(fileNumber);
        ++fileNumber;
    }

    if(isAllRequestSuccessful == false)
        emit finished(false);
    else
        emit finished(true);
}
