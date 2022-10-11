#include "TaskAddNewFolders.h"

#include "Backend/FileStorageSubSystem/FileStorageManager.h"

#include <QDir>

TaskAddNewFolders::TaskAddNewFolders(const QList<DialogAddNewFolder::FolderItem> &_list, QObject *parent)
    : QThread{parent}
{
    list = _list;
}

TaskAddNewFolders::~TaskAddNewFolders()
{
}

int TaskAddNewFolders::fileCount() const
{
    int result = 0;

    for(const DialogAddNewFolder::FolderItem &item : list)
        result += item.files.size();

    return result;
}

void TaskAddNewFolders::run()
{
    auto fsm = FileStorageManager::instance();
    int fileNumber = 1;
    bool isAllRequestSuccessful = true;

    // first create folders
    for(const DialogAddNewFolder::FolderItem &item : list)
    {
        fsm->addNewFolder(item.symbolDir, item.userDir);

        // Then add files
        QHashIterator<QString, bool> cursor(item.files);
        while(cursor.hasNext())
        {
            cursor.next();

            emit signalFileBeingProcessed(cursor.key());
            emit signalGenericFileEvent();

            bool requestResult = fsm->addNewFile(cursor.key(), item.symbolDir, false, cursor.value(), item.userDir);

            if(requestResult == true)
                emit signalFileAddedSuccessfully(cursor.key());
            else
            {
                isAllRequestSuccessful = false;
                emit signalFileAddingFailed(cursor.key());
            }

            emit signalFileProcessed(fileNumber);
            emit signalGenericFileEvent();
            ++fileNumber;
        }
    }

    if(isAllRequestSuccessful == false)
        emit finished(false);
    else
        emit finished(true);
}
