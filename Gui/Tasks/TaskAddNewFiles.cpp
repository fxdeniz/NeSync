#include "TaskAddNewFiles.h"

#include <QDir>
#include <QStandardPaths>

TaskAddNewFiles::TaskAddNewFiles(QObject *parent)
    : QObject{parent}
{
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

const QString &TaskAddNewFiles::getTargetSymbolFolder() const
{
    return targetSymbolFolder;
}

void TaskAddNewFiles::setTargetSymbolFolder(const QString &newTargetSymbolFolder)
{
    targetSymbolFolder = newTargetSymbolFolder;
}

void TaskAddNewFiles::run()
{
    for(const QString &currentFilePath : fileSet)
    {
        QFileInfo fileInfo(currentFilePath);
        QString userDirectory = QDir::toNativeSeparators(fileInfo.absolutePath()) + QDir::separator();

        bool requestResult = this->fsm->addNewFile(currentFilePath,
                                                  this->getTargetSymbolFolder(),
                                                  false,
                                                  true,
                                                  userDirectory);
    }
}
