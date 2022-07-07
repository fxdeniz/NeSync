#include "TaskAddNewFiles.h"

#include <QDir>
#include <QStandardPaths>

TaskAddNewFiles::TaskAddNewFiles(const QString &targetSymbolFolder, QObject *parent)
    : QObject{parent}
{
    this->targetSymbolFolder = targetSymbolFolder;

    auto appDataDir = QStandardPaths::writableLocation(QStandardPaths::StandardLocation::TempLocation);
    appDataDir = QDir::toNativeSeparators(appDataDir);
    appDataDir += QDir::separator();

    auto backupDir = appDataDir + "backup" + QDir::separator();
    auto symbolDir = appDataDir + "symbols" + QDir::separator();
    QDir dir;
    dir.mkdir(backupDir);
    dir.mkdir(symbolDir);

    this->fsm = new FileStorageManager(backupDir, symbolDir, this);
}

void TaskAddNewFiles::addFile(const QString &pathToFile)
{
    if(!fileSet.contains(pathToFile))
        fileSet.insert(pathToFile);
}

int TaskAddNewFiles::fileCount() const
{
    return this->fileSet.size();
}

const QString &TaskAddNewFiles::getTargetSymbolFolder() const
{
    return targetSymbolFolder;
}

void TaskAddNewFiles::run()
{
    for(const QString &currentFilePath : qAsConst(fileSet))
    {
        QFileInfo fileInfo(currentFilePath);
        QString userDirectory = QDir::toNativeSeparators(fileInfo.absolutePath()) + QDir::separator();

        bool requestResult = this->fsm->addNewFile(currentFilePath,
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
