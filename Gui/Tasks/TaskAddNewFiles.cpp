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

void TaskAddNewFiles::run()
{

}
