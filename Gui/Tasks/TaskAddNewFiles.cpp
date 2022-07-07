#include "TaskAddNewFiles.h"

#include <QDir>
#include <QStandardPaths>

TaskAddNewFiles::TaskAddNewFiles(const QString &targetSymbolFolder, QStringList fileList, QObject *parent)
    : QObject{parent}
{
    this->targetSymbolFolder = targetSymbolFolder;
    this->fileList = fileList;
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
            emit signalFileAddingFailed(currentFilePath);
    }
}
