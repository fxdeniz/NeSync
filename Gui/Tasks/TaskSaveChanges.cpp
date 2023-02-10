#include "TaskSaveChanges.h"

#include "Backend/FileStorageSubSystem/FileStorageManager.h"
#include "Utility/DatabaseRegistry.h"
#include "Utility/JsonDtoFormat.h"

#include <QDir>
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
    saveFolderChanges();
    saveFileChanges();
}

void TaskSaveChanges::saveFolderChanges()
{
    auto fsm = FileStorageManager::instance();

    while(folderItemIterator.hasNext())
    {
        folderItemIterator.next();
        TreeItem *item = folderItemIterator.value();
        TreeItem::Action action = item->getAction();

        if(action == TreeItem::Action::Save)
        {
            QJsonObject parentFolderJson = fsm->getFolderJsonByUserPath(item->getParentItem()->getUserPath());
            QString symbolFolderPath = parentFolderJson[JsonKeys::Folder::SymbolFolderPath].toString();
            symbolFolderPath += QDir(item->getUserPath()).dirName();

            fsm->addNewFolder(symbolFolderPath, item->getUserPath());
        }
    }
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
        TreeItem::Action action = item->getAction();

        if(fileJson[JsonKeys::IsExist].toBool()) // If file info exist in db
        {
            if(action == TreeItem::Action::Delete)
                fsm->deleteFile(symbolFilePath);
            else if(action == TreeItem::Action::Save) // Saves FileSystemEventDb::ItemStatus::Updated files
                fsm->appendVersion(symbolFilePath, item->getUserPath(), item->getDescription());
            else if(action == TreeItem::Action::Freeze) // Freezes FileSystemEventDb::ItemStatus::Deleted files
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
        else // If file info NOT exist in db
        {
            FileSystemEventDb fsEventDb(DatabaseRegistry::fileSystemEventDatabase());
            QString userPathToOldFile = item->getParentItem()->getUserPath() + fsEventDb.getOldNameOfFile(item->getUserPath());
            fileJson = fsm->getFileJsonByUserPath(userPathToOldFile);
            symbolFilePath = fileJson[JsonKeys::File::SymbolFilePath].toString();

            if(action == TreeItem::Action::Restore) // Restores FileSystemEventDb::ItemStatus::Renamed and UpdatedAndRenamed files
            {
                qlonglong maxVersionNumber = fileJson[JsonKeys::File::MaxVersionNumber].toInteger();
                QJsonObject versionJson = fsm->getFileVersionJson(symbolFilePath, maxVersionNumber);
                QString internalFileName = versionJson[JsonKeys::FileVersion::InternalFileName].toString();
                auto internalFilePath = fsm->getBackupFolderPath() + internalFileName;
                QString userFilePath = fileJson[JsonKeys::File::UserFilePath].toString();

                QFile::copy(internalFilePath, userFilePath);
                QFile::remove(item->getUserPath());
            }
            else if(action == TreeItem::Action::Save)
            {
                if(status == FileSystemEventDb::ItemStatus::NewAdded)
                {
                    QJsonObject folderJson = fsm->getFolderJsonByUserPath(item->getParentItem()->getUserPath());
                    fsm->addNewFile(folderJson[JsonKeys::Folder::SymbolFolderPath].toString(), item->getUserPath());
                }
                else if(status == FileSystemEventDb::ItemStatus::Renamed ||
                        status == FileSystemEventDb::ItemStatus::UpdatedAndRenamed)
                {
                    if(status == FileSystemEventDb::ItemStatus::UpdatedAndRenamed)
                        fsm->appendVersion(symbolFilePath, item->getUserPath(), item->getDescription());

                    // Rename file
                    fileJson[JsonKeys::File::FileName] = fsEventDb.getNameOfFile(item->getUserPath());
                    fsm->updateFileEntity(fileJson);
                }
            }
        }
    }
}
