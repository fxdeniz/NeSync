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
    FileSystemEventDb fsEventDb(DatabaseRegistry::fileSystemEventDatabase());

    while(folderItemIterator.hasNext())
    {
        folderItemIterator.next();
        TreeItem *item = folderItemIterator.value();
        QJsonObject folderJson = fsm->getFolderJsonByUserPath(item->getUserPath());
        FileSystemEventDb::ItemStatus status = item->getStatus();
        TreeItem::Action action = item->getAction();
        QDir dir(item->getUserPath());

        if(folderJson[JsonKeys::IsExist].toBool()) // If folder info exist in db
        {
            if(action == TreeItem::Action::Restore) // Restore deleted folders
            {
                bool isCreated = dir.mkpath(item->getUserPath());
                if(isCreated)
                    emit folderRestored(item->getUserPath());
            }
            else if(action == TreeItem::Action::Delete) // Remove deleted folders from db
            {
                bool isRemoved = fsm->deleteFolder(folderJson[JsonKeys::Folder::SymbolFolderPath].toString());
                if(isRemoved)
                    fsEventDb.deleteFolder(item->getUserPath());
            }
            else if(action ==  TreeItem::Action::Freeze) // Freeze deleted folders
            {
                folderJson[JsonKeys::Folder::IsFrozen] = true;
                bool isUpdated = fsm->updateFolderEntity(folderJson, true);
                if(isUpdated)
                    fsEventDb.deleteFolder(item->getUserPath());
            }
        }
        else // If folder info not exist in db
        {
            QJsonObject parentFolderJson = fsm->getFolderJsonByUserPath(item->getParentItem()->getUserPath());
            QString symbolFolderPath = parentFolderJson[JsonKeys::Folder::SymbolFolderPath].toString();

            if(action == TreeItem::Action::Save)
            {
                if(status == FileSystemEventDb::ItemStatus::NewAdded) // Save newly added folders
                {
                    symbolFolderPath += dir.dirName();
                    bool isSaved = fsm->addNewFolder(symbolFolderPath, item->getUserPath());
                    if(isSaved)
                        fsEventDb.setStatusOfFolder(item->getUserPath(), FileSystemEventDb::ItemStatus::Monitored);
                }
                else if(status == FileSystemEventDb::ItemStatus::Renamed)
                {
                    symbolFolderPath += fsEventDb.getOldNameOfFolder(item->getUserPath());
                    symbolFolderPath += FileStorageManager::separator;

                    folderJson = fsm->getFolderJsonBySymbolPath(symbolFolderPath);
                    folderJson[JsonKeys::Folder::SuffixPath] = dir.dirName();
                    folderJson[JsonKeys::Folder::UserFolderPath] = item->getUserPath();

                    bool isSaved = fsm->updateFolderEntity(folderJson);

                    if(isSaved)
                    {
                        fsEventDb.setOldNameOfFolder(item->getUserPath(), "");
                        fsEventDb.setStatusOfFolder(item->getUserPath(), FileSystemEventDb::ItemStatus::Monitored);
                    }
                }
            }
            else if(action == TreeItem::Action::Restore) // Restore renamed folders
            {
                QString oldFolderPath = parentFolderJson[JsonKeys::Folder::UserFolderPath].toString();
                oldFolderPath += fsEventDb.getOldNameOfFolder(item->getUserPath());
                dir.removeRecursively();
                bool isCreated = dir.mkpath(oldFolderPath);

                if(isCreated)
                {
                    fsEventDb.setOldNameOfFolder(item->getUserPath(), "");
                    fsEventDb.setStatusOfFolder(item->getUserPath(), FileSystemEventDb::ItemStatus::Monitored);
                    fsEventDb.setPathOfFolder(item->getUserPath(), oldFolderPath);
                }
            }
        }
    }
}

void TaskSaveChanges::saveFileChanges()
{
    auto fsm = FileStorageManager::instance();
    FileSystemEventDb fsEventDb(DatabaseRegistry::fileSystemEventDatabase());

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
            {
                bool isDeleted = fsm->deleteFile(symbolFilePath);
                if(isDeleted)
                    fsEventDb.deleteFile(item->getUserPath());
            }
            else if(action == TreeItem::Action::Save) // Saves FileSystemEventDb::ItemStatus::Updated files
            {
                bool isAppended = fsm->appendVersion(symbolFilePath, item->getUserPath(), item->getDescription());
                if(isAppended)
                    fsEventDb.setStatusOfFile(item->getUserPath(), FileSystemEventDb::ItemStatus::Monitored);
            }
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

                QFile::remove(item->getUserPath()); // If restored file exist remove it
                QFile::copy(internalFilePath, userFilePath);
            }
        }
        else // If file info NOT exist in db
        {
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
