#include "TaskSaveChanges.h"

#include "Backend/FileStorageSubSystem/FileStorageManager.h"
#include "Utility/DatabaseRegistry.h"
#include "Utility/JsonDtoFormat.h"

#include <QFile>

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
    saveFileChanges();
}

void TaskSaveChanges::saveFileChanges()
{
    auto fsm = FileStorageManager::instance();

    while(fileItemIterator.hasNext())
    {
        fileItemIterator.next();
        TreeItem *item = fileItemIterator.value();
        QJsonObject fileJson = fsm->getFileJsonByUserPath(fileItemIterator.key());
        QString symbolFilePath = fileJson[JsonKeys::File::SymbolFilePath].toString();
        FileSystemEventDb::ItemStatus status = item->getStatus();

        if(fileJson[JsonKeys::IsExist].toBool()) // If file is persists
        {
            TreeItem::Action action = item->getAction();

            if(action == TreeItem::Action::Delete)
                fsm->deleteFile(symbolFilePath);
            else if(action == TreeItem::Action::Save)
            {
                if(status == FileSystemEventDb::ItemStatus::Updated)
                    fsm->appendVersion(symbolFilePath, item->getUserPath(), item->getDescription());
            }
            else if(action == TreeItem::Action::Freeze)
            {
                fileJson[JsonKeys::File::IsFrozen] = true;
                fsm->updateFileEntity(fileJson);
            }
            else if(action == TreeItem::Action::Restore)
            {
                qlonglong maxVersionNumber = fileJson[JsonKeys::File::MaxVersionNumber].toInteger();
                QJsonObject versionJson = fsm->getFileVersionJson(symbolFilePath, maxVersionNumber);
                QString internalFileName = versionJson[JsonKeys::FileVersion::InternalFileName].toString();
                auto internalFilePath = fsm->getBackupFolderPath() + internalFileName;
                QString userFilePath = fileJson[JsonKeys::File::UserFilePath].toString();

                QFile::copy(internalFilePath, userFilePath);
            }
        }
        else // If file is NOT persists
        {
            if(status == FileSystemEventDb::ItemStatus::NewAdded)
            {
                QJsonObject folderJson = fsm->getFolderJsonByUserPath(item->getParentItem()->getUserPath());
                fsm->addNewFile(folderJson[JsonKeys::Folder::SymbolFolderPath].toString(), item->getUserPath());
            }
            else if(status == FileSystemEventDb::ItemStatus::Renamed ||
                    status == FileSystemEventDb::ItemStatus::UpdatedAndRenamed)
            {
                FileSystemEventDb fsEventDb(DatabaseRegistry::fileSystemEventDatabase());
                QString userPathToOldFile = item->getParentItem()->getUserPath() + fsEventDb.getOldNameOfFile(item->getUserPath());
                fileJson = fsm->getFileJsonByUserPath(userPathToOldFile);

                if(status == FileSystemEventDb::ItemStatus::UpdatedAndRenamed)
                {
                    fsm->appendVersion(fileJson[JsonKeys::File::SymbolFilePath].toString(),
                                       item->getUserPath(),
                                       item->getDescription());
                }

                // Rename file
                fileJson[JsonKeys::File::FileName] = fsEventDb.getNameOfFile(item->getUserPath());
                fsm->updateFileEntity(fileJson);
            }
        }
    }
}
