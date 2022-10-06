#include "TaskAddNewFolders.h"

#include "Backend/FileStorageSubSystem/FileStorageManager.h"

#include <QDir>

TaskAddNewFolders::TaskAddNewFolders(const QHash<QString, QString> &userDirToSymbolDirMapping,
                                     QObject *parent)
    : QThread{parent}
{
    hashIterator = new QHashIterator<QString, QString>(userDirToSymbolDirMapping);
}

TaskAddNewFolders::~TaskAddNewFolders()
{
    delete hashIterator;
}

void TaskAddNewFolders::addFile(const QString &userFilePath, bool isAutoSyncEnabled)
{
    bool isContains = files.contains(userFilePath);

    if(!isContains)
        files.insert(userFilePath, isAutoSyncEnabled);
}

int TaskAddNewFolders::fileCount() const
{
    return files.size();
}

void TaskAddNewFolders::run()
{

}

void TaskAddNewFolders::createSymbolDirs()
{
    auto fsm = FileStorageManager::instance();

    while(hashIterator->hasNext())
    {
        // QHashIterator starts from index -1
        //      see https://doc.qt.io/qt-6/qhashiterator.html#details
        hashIterator->next();

        auto currentSymbolDir = hashIterator->value();
        currentSymbolDir.replace(QDir::separator(), FileStorageManager::CONST_SYMBOL_DIRECTORY_SEPARATOR);
        fsm->addNewFolder(currentSymbolDir, hashIterator->key());
    }
}
