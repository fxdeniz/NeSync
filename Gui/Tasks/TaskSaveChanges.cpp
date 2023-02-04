#include "TaskSaveChanges.h"

#include "Backend/FileStorageSubSystem/FileStorageManager.h"
#include "Utility/JsonDtoFormat.h"

TaskSaveChanges::TaskSaveChanges(const QHash<QString, TreeItem *> folderItemMap,
                                 const QHash<QString, TreeItem *> fileItemMap,
                                 QObject *parent)
    : QThread{parent}, folderItemIterator(folderItemMap), fileItemIterator(fileItemMap)
{
}

TaskSaveChanges::~TaskSaveChanges()
{

}

void TaskSaveChanges::run()
{
    auto fsm = FileStorageManager::instance();

    while(fileItemIterator.hasNext())
    {
        fileItemIterator.next();
        QJsonObject fileJson = fsm->getFileJsonByUserPath(fileItemIterator.key());

        if(fileJson[JsonKeys::IsExist].toBool())
        {
            fsm->appendVersion(fileJson[JsonKeys::File::SymbolFilePath].toString(),
                               fileItemIterator.key(),
                               fileItemIterator.value()->getDescription());
        }
    }
}
