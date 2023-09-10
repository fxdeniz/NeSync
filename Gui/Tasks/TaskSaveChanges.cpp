#include "TaskSaveChanges.h"

#include "Backend/FileStorageSubSystem/FileStorageManager.h"
#include "Utility/DatabaseRegistry.h"
#include "Utility/JsonDtoFormat.h"

#include <QDir>
#include <QFile>

TaskSaveChanges::TaskSaveChanges(const QMap<QString, TreeModelFileMonitor::TreeItem *> folderItemMap,
                                 const QMap<QString, TreeModelFileMonitor::TreeItem *> fileItemMap,
                                 QObject *parent)
    : QThread{parent}, folderItemIterator(folderItemMap), fileItemIterator(fileItemMap)
{
    totalItemCount = folderItemMap.size() + fileItemMap.size();
    currentItemNumber = 0;
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

        ++currentItemNumber;
        emit itemBeingProcessed(currentItemNumber);

        TreeModelFileMonitor::TreeItem *item = folderItemIterator.value();
        QJsonObject folderJson = fsm->getFolderJsonByUserPath(item->getUserPath());
        FileSystemEventDb::ItemStatus status = item->getStatus();
        TreeModelFileMonitor::TreeItem::Action action = item->getAction();
        QDir dir(item->getUserPath());

        if(folderJson[JsonKeys::IsExist].toBool()) // If folder info exist in db
        {
            if(action == TreeModelFileMonitor::TreeItem::Action::Restore) // Restore deleted folders
            {
                bool isCreated = dir.mkpath(item->getUserPath());
                if(isCreated)
                    emit folderRestored(item->getUserPath());
            }
            else if(action == TreeModelFileMonitor::TreeItem::Action::Delete) // Remove deleted folders from db
            {
                bool isRemoved = fsm->deleteFolder(folderJson[JsonKeys::Folder::SymbolFolderPath].toString());
                if(isRemoved)
                    fsEventDb.deleteFolder(item->getUserPath());
            }
            else if(action ==  TreeModelFileMonitor::TreeItem::Action::Freeze) // Freeze deleted folders
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

            if(action == TreeModelFileMonitor::TreeItem::Action::Save)
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
            else if(action == TreeModelFileMonitor::TreeItem::Action::Restore) // Restore renamed folders
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

        ++currentItemNumber;
        emit itemBeingProcessed(currentItemNumber);

        TreeModelFileMonitor::TreeItem *item = fileItemIterator.value();
        QJsonObject fileJson = fsm->getFileJsonByUserPath(fileItemIterator.key());
        QString symbolFilePath = fileJson[JsonKeys::File::SymbolFilePath].toString();
        FileSystemEventDb::ItemStatus status = item->getStatus();
        TreeModelFileMonitor::TreeItem::Action action = item->getAction();

        if(fileJson[JsonKeys::IsExist].toBool()) // If file info exist in db
        {
            if(action == TreeModelFileMonitor::TreeItem::Action::Delete)
            {
                bool isDeleted = fsm->deleteFile(symbolFilePath);
                if(isDeleted)
                    fsEventDb.deleteFile(item->getUserPath());
            }
            else if(action == TreeModelFileMonitor::TreeItem::Action::Save) // Saves FileSystemEventDb::ItemStatus::Updated files
            {
                bool isAppended = fsm->appendVersion(symbolFilePath, item->getUserPath(), item->getDescription());
                if(isAppended)
                    fsEventDb.setStatusOfFile(item->getUserPath(), FileSystemEventDb::ItemStatus::Monitored);
            }
            else if(action == TreeModelFileMonitor::TreeItem::Action::Freeze) // Freezes FileSystemEventDb::ItemStatus::Deleted files
            {
                fileJson[JsonKeys::File::IsFrozen] = true;
                bool isUpdated = fsm->updateFileEntity(fileJson);
                if(isUpdated)
                    fsEventDb.deleteFile(item->getUserPath());
            }
            else if(action == TreeModelFileMonitor::TreeItem::Action::Restore)
            {
                qlonglong maxVersionNumber = fileJson[JsonKeys::File::MaxVersionNumber].toInteger();
                QJsonObject versionJson = fsm->getFileVersionJson(symbolFilePath, maxVersionNumber);
                QString internalFileName = versionJson[JsonKeys::FileVersion::InternalFileName].toString();
                QString internalFilePath = fsm->getStorageFolderPath() + internalFileName;
                QString userFilePath = fileJson[JsonKeys::File::UserFilePath].toString();

                QFile::remove(item->getUserPath()); // If restored file exist remove it
                bool isCopied = QFile::copy(internalFilePath, userFilePath);

                QFile file(userFilePath);
                file.open(QFile::OpenModeFlag::Append);
                QString strLastModifiedTimestamp = versionJson[JsonKeys::FileVersion::LastModifiedTimestamp].toString();
                QDateTime lastModifiedTimestamp = QDateTime::fromString(strLastModifiedTimestamp, Qt::DateFormat::ISODateWithMs);
                bool isTimestampSet = file.setFileTime(lastModifiedTimestamp, QFileDevice::FileTime::FileModificationTime);

                if(isCopied && isTimestampSet)
                    fsEventDb.setStatusOfFile(item->getUserPath(), FileSystemEventDb::ItemStatus::Monitored);
            }
        }
        else // If file info NOT exist in db
        {
            QString userPathToOldFile = item->getParentItem()->getUserPath() + fsEventDb.getOldNameOfFile(item->getUserPath());
            fileJson = fsm->getFileJsonByUserPath(userPathToOldFile);
            symbolFilePath = fileJson[JsonKeys::File::SymbolFilePath].toString();

            if(action == TreeModelFileMonitor::TreeItem::Action::Restore) // Restores FileSystemEventDb::ItemStatus::Renamed and UpdatedAndRenamed files
            {
                qlonglong maxVersionNumber = fileJson[JsonKeys::File::MaxVersionNumber].toInteger();
                QJsonObject versionJson = fsm->getFileVersionJson(symbolFilePath, maxVersionNumber);
                QString internalFileName = versionJson[JsonKeys::FileVersion::InternalFileName].toString();
                auto internalFilePath = fsm->getStorageFolderPath() + internalFileName;
                QString userFilePath = fileJson[JsonKeys::File::UserFilePath].toString();

                QFile::remove(item->getUserPath());
                bool isCopied = QFile::copy(internalFilePath, userFilePath);
                if(isCopied)
                    fsEventDb.setStatusOfFile(item->getUserPath(), FileSystemEventDb::ItemStatus::Monitored);
            }
            else if(action == TreeModelFileMonitor::TreeItem::Action::Save)
            {
                if(status == FileSystemEventDb::ItemStatus::NewAdded)
                {
                    QJsonObject folderJson = fsm->getFolderJsonByUserPath(item->getParentItem()->getUserPath());
                    bool isAdded = fsm->addNewFile(folderJson[JsonKeys::Folder::SymbolFolderPath].toString(), item->getUserPath());
                    if(isAdded)
                        fsEventDb.setStatusOfFile(item->getUserPath(), FileSystemEventDb::ItemStatus::Monitored);
                }
                else if(status == FileSystemEventDb::ItemStatus::Renamed)
                {
                    // Rename file
                    fileJson[JsonKeys::File::FileName] = fsEventDb.getNameOfFile(item->getUserPath());
                    bool isUpdated = fsm->updateFileEntity(fileJson);
                    if(isUpdated)
                    {
                        fsEventDb.setOldNameOfFile(item->getUserPath(), "");
                        fsEventDb.setStatusOfFile(item->getUserPath(), FileSystemEventDb::ItemStatus::Monitored);
                    }
                }
            }
        }
    }
}

int TaskSaveChanges::getTotalItemCount() const
{
    return totalItemCount;
}
