#include "TaskAddNewFolders.h"

#include "Backend/FileStorageSubSystem/FileStorageManager.h"

#include <QDir>

TaskAddNewFolders::TaskAddNewFolders(const QList<V2_DialogAddNewFolder::FolderItem> &_list, QObject *parent)
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

    for(const V2_DialogAddNewFolder::FolderItem &item : list)
        result += item.files.size();

    return result;
}

void TaskAddNewFolders::run()
{
    auto fsm = FileStorageManager::instance();

    // first create folders
    for(const V2_DialogAddNewFolder::FolderItem &item : list)
    {
        fsm->addNewFolder(item.symbolDir, item.userDir);

        // Then add files
        QHashIterator<QString, bool> cursor(item.files);
        while(cursor.hasNext())
        {
            cursor.next();
            fsm->addNewFile(cursor.key(), item.symbolDir, false, cursor.value(), item.userDir);
        }
    }
}
