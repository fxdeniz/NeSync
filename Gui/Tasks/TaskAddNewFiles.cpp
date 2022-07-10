#include "TaskAddNewFiles.h"

#include <QDir>
#include <QStandardPaths>

TaskAddNewFiles::TaskAddNewFiles(const QString &targetSymbolFolder, QObject *parent)
    : QThread{parent}
{
    this->targetSymbolFolder = targetSymbolFolder;
}

TaskAddNewFiles::~TaskAddNewFiles()
{
}

void TaskAddNewFiles::addAutoSyncEnabled(const QString &pathToFile)
{
    bool isContains = this->fileMap.contains(pathToFile);

    if(!isContains)
        this->fileMap.insert(pathToFile, true);
}

void TaskAddNewFiles::addAutoSyncDisabled(const QString &pathToFile)
{
    bool isContains = this->fileMap.contains(pathToFile);

    if(!isContains)
        this->fileMap.insert(pathToFile, false);
}

int TaskAddNewFiles::fileCount() const
{
    return this->fileMap.size();
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

    QHashIterator<QString, bool> cursor(this->fileMap);
    while(cursor.hasNext())
    {
        cursor.next();

        QString currentFilePath = cursor.key();
        bool isAutoSyncEnabled = cursor.value();
        QFileInfo fileInfo(currentFilePath);
        QString userDirectory = QDir::toNativeSeparators(fileInfo.absolutePath()) + QDir::separator();

        emit signalFileBeingProcessed(currentFilePath);

        bool requestResult = fsm->addNewFile(currentFilePath,
                                             this->getTargetSymbolFolder(),
                                             false,
                                             isAutoSyncEnabled,
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
