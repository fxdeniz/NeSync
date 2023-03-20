#include "TaskAddNewFolders.h"

#include "Backend/FileStorageSubSystem/FileStorageManager.h"

#include <QDir>

TaskAddNewFolders::TaskAddNewFolders(QList<DialogAddNewFolder::FolderItem> list, QObject *parent)
    : QThread{parent}
{
    this->list = list;
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

    // First create folders
    for(const DialogAddNewFolder::FolderItem &item : list)
    {
        fsm->addNewFolder(item.symbolFolderPath, item.userFolderPath);
        emit signalFolderAdded(item.userFolderPath);

        // Then add files
        QHashIterator<QString, bool> cursor(item.files);
        while(cursor.hasNext())
        {
            cursor.next();

            emit signalFileBeingProcessed(cursor.key());
            emit signalGenericFileEvent();

            QString description = "Initial version of <b>%1</b>";
            description = description.arg(QFileInfo(cursor.key()).fileName());
            bool requestResult = fsm->addNewFile(item.symbolFolderPath, cursor.key(), cursor.value(), "", description);

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
