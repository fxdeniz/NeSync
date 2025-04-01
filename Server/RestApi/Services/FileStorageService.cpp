#include "FileStorageService.h"

#include "JsonDtoFormat.h"
#include "FileStorageSubSystem/FileStorageManager.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>

FileStorageService::FileStorageService(QObject *parent)
    : QObject{parent}
{}

bool FileStorageService::deleteFolder(const QString &symbolFolderPath)
{
    auto fsm = FileStorageManager::instance();

    QJsonObject dto = fsm->getFolderJsonBySymbolPath(symbolFolderPath);
    bool isDeletedFromDb = fsm->deleteFolder(symbolFolderPath);

    if(!isDeletedFromDb)
        return false;

    _lastSymbolFolderPath = dto[JsonKeys::File::SymbolFolderPath].toString();

    // Delete whether existing folder is frozen or not.
    QDir dir(dto[JsonKeys::Folder::UserFolderPath].toString());
    dir.removeRecursively(); // TODO: Synchronize result of this operation with the db using transactions.

    return true;
}

// TODO: Re-write this function using transactions.
bool FileStorageService::renameFolder(const QString &symbolFolderPath, QString folderName)
{
    auto fsm = FileStorageManager::instance();

    QJsonObject parentDto = fsm->getFolderJsonBySymbolPath(symbolFolderPath, true);
    QString oldName = parentDto[JsonKeys::Folder::SuffixPath].toString();
    QString newUserPath = parentDto[JsonKeys::Folder::UserFolderPath].toString().split(oldName).first();
    newUserPath += folderName;
    newUserPath = QDir::toNativeSeparators(newUserPath);
    bool isFrozen = parentDto[JsonKeys::Folder::IsFrozen].toBool();

    if(!folderName.endsWith(fsm->separator))
        return false;

    if(!isFrozen)
    {
        QJsonArray children = parentDto[JsonKeys::Folder::ChildFolders].toArray();
        while(!children.isEmpty())
        {
            QString currentUserPath;
            QJsonObject dto = children.first().toObject();
            dto = fsm->getFolderJsonBySymbolPath(dto[JsonKeys::Folder::SymbolFolderPath].toString(), true);
            currentUserPath = dto[JsonKeys::Folder::UserFolderPath].toString();

            qlonglong index = currentUserPath.indexOf(oldName);

            if (index != -1) // TODO: Cancel transaction
            {
                currentUserPath.replace(index, oldName.length(), folderName);
                currentUserPath = QDir::toNativeSeparators(currentUserPath);
            }

            dto[JsonKeys::Folder::UserFolderPath] = currentUserPath;

            bool isChildRenamed = fsm->updateFolderEntity(dto);

            if(!isChildRenamed)
                return false;

            QJsonArray subChildren = dto[JsonKeys::Folder::ChildFolders].toArray();
            if(!subChildren.isEmpty())
                for(const QJsonValue &value : subChildren)
                    children.append(value.toObject());

            children.removeFirst();
        }
    }

    parentDto[JsonKeys::Folder::SuffixPath] = folderName;
    parentDto[JsonKeys::Folder::UserFolderPath] = newUserPath;
    bool isParentRenamed = fsm->updateFolderEntity(parentDto);

    if(!isParentRenamed)
        return false;

    _lastSymbolFolderPath = parentDto[JsonKeys::Folder::ParentFolderPath].toString() + folderName;

    if(isFrozen)
        return true;

    QString userPath = parentDto[JsonKeys::Folder::UserFolderPath].toString();

    QDir dir(userPath);
    dir.cdUp();

    bool result = dir.rename(oldName.chopped(1), folderName.chopped(1)); // TODO: Synchronize result of this operation with the db using transactions.
    return result;
}

bool FileStorageService::renameFile(const QString &symbolFilePath, const QString &fileName)
{
    auto fsm = FileStorageManager::instance();

    QJsonObject dto = fsm->getFileJsonBySymbolPath(symbolFilePath);

    dto[JsonKeys::File::FileName] = fileName;
    bool isRenamedInDb = fsm->updateFileEntity(dto);

    if(!isRenamedInDb)
        return false;

    _lastSymbolFilePath = dto[JsonKeys::File::SymbolFolderPath].toString() + fileName;

    bool isFrozen = dto[JsonKeys::File::IsFrozen].toBool();

    if(isFrozen)
        return true;

    QString userPath = dto[JsonKeys::File::UserFilePath].toString();

    QFile file(userPath);
    QFileInfo fileInfo(userPath);
    QString newPath = fileInfo.dir().filePath(fileName);

    bool result = file.rename(newPath); // TODO: Synchronize result of this operation with the db using transactions.
    return result;
}

bool FileStorageService::freezeFolder(const QString &symbolFolderPath)
{
    auto fsm = FileStorageManager::instance();

    QJsonObject dto = fsm->getFolderJsonBySymbolPath(symbolFolderPath);

    QString userPath = dto[JsonKeys::Folder::UserFolderPath].toString();
    dto[JsonKeys::Folder::IsFrozen] = true;

    bool isUpdated = fsm->updateFolderEntity(dto, true);
    QDir dir(userPath);

    if(isUpdated && dir.exists())
        dir.removeRecursively();

    return isUpdated;
}

// TODO: Prevent child folder being mapped outside the parent folder.
//       For example, consider this hierarchy:
//       - Other Folder
//          - Some Folder
//
//      - Folder A
//          - Folder B
//
//     This should not happen:
//       - Other Folder
//          - Some Folder
//          - Folder B
//
//      - Folder A
//
// TODO: Prevent relocating children of frozen parent.
// TODO: Improve overwriting logic to prevent accidental deletes.
bool FileStorageService::relocateFolder(const QString &symbolFolderPath, const QString &rootUserPath)
{
    auto fsm = FileStorageManager::instance();
    QDir destination(rootUserPath);

    if(destination.exists()) // Prepare for overwrite.
    {
        bool isRemoved = destination.removeRecursively();

        if(!isRemoved)
            return false;
    }

    QJsonObject dto = fsm->getFolderJsonBySymbolPath(symbolFolderPath);

    dto[JsonKeys::Folder::IsFrozen] = false;

    bool isUpdated = fsm->updateFolderEntity(dto, true);

    if(!isUpdated)
        return false;

    QJsonArray tree {dto};
    bool isCreatingRoot = true;

    while(!tree.isEmpty())
    {
        QJsonObject folder = tree.first().toObject();

        folder = fsm->getFolderJsonBySymbolPath(folder[JsonKeys::Folder::SymbolFolderPath].toString(), true);
        QJsonArray childFolders = folder[JsonKeys::Folder::ChildFolders].toArray();

        if(!childFolders.isEmpty())
        {
            for(const QJsonValue &value : childFolders)
                tree.append(value.toObject());
        }

        if(isCreatingRoot)
        {
            folder[JsonKeys::Folder::UserFolderPath] = rootUserPath;
            isCreatingRoot = false;
        }
        else
        {
            QJsonObject parent = fsm->getFolderJsonBySymbolPath(folder[JsonKeys::Folder::ParentFolderPath].toString());
            QString path = parent[JsonKeys::Folder::UserFolderPath].toString();
            path += folder[JsonKeys::Folder::SuffixPath].toString();
            folder[JsonKeys::Folder::UserFolderPath] = path;
        }

        bool isFolderUpdated = fsm->updateFolderEntity(folder);

        if(!isFolderUpdated)
            return false;
        else
        {
            QString path = folder[JsonKeys::Folder::UserFolderPath].toString();
            bool isFolderCreated = QDir(path).mkdir(path);

            if(!isFolderCreated)
                return false;
            else
            {
                QJsonArray files = folder[JsonKeys::Folder::ChildFiles].toArray();

                for(const QJsonValue &value : files)
                {
                    QJsonObject file = value.toObject();
                    file = fsm->getFileJsonBySymbolPath(file[JsonKeys::File::SymbolFilePath].toString(), true);

                    QJsonObject latestVersion = file[JsonKeys::File::VersionList].toArray().last().toObject();

                    QString dest = fsm->getStorageFolderPath() + latestVersion[JsonKeys::FileVersion::InternalFileName].toString();
                    bool isFileCopied = QFile::copy(dest,file[JsonKeys::File::UserFilePath].toString());

                    QFile copiedFile(file[JsonKeys::File::UserFilePath].toString());
                    copiedFile.open(QFile::OpenModeFlag::ReadWrite);

                    QDateTime timestamp = QDateTime::fromString(latestVersion[JsonKeys::FileVersion::LastModifiedTimestamp].toString(),
                                                                Qt::DateFormat::ISODateWithMs);

                    // TODO: On linux, check result of this.
                    copiedFile.setFileTime(timestamp, QFileDevice::FileTime::FileModificationTime);

                    if(!isFileCopied)
                        return false;
                }
            }
        }

        tree.removeFirst();
    }

    return true;
}

// TODO: Prevent relocating child files of frozen parent.
// TODO: Improve overwriting logic to prevent accidental deletes.
bool FileStorageService::relocateFile(const QString &symbolFilePath)
{
    auto fsm = FileStorageManager::instance();
    QJsonObject file = fsm->getFileJsonBySymbolPath(symbolFilePath, true);
    QJsonObject latest = fsm->getFileVersionJson(symbolFilePath, file[JsonKeys::File::MaxVersionNumber].toInteger());
    QJsonObject parent = fsm->getFolderJsonBySymbolPath(file[JsonKeys::File::SymbolFolderPath].toString());

    QString userFilePath = parent[JsonKeys::Folder::UserFolderPath].toString() + file[JsonKeys::File::FileName].toString();
    QFile previous(userFilePath);

    if(previous.exists())
        previous.remove();

    bool isCopied = QFile::copy(fsm->getStorageFolderPath() + latest[JsonKeys::FileVersion::InternalFileName].toString(),
                                userFilePath);

    if(!isCopied)
        return false;

    QFile destination(userFilePath);
    QDateTime timestamp = QDateTime::fromString(latest[JsonKeys::FileVersion::LastModifiedTimestamp].toString(),
                                                Qt::DateFormat::ISODateWithMs);

    destination.open(QFile::OpenModeFlag::ReadWrite);
    destination.setFileTime(timestamp, QFileDevice::FileTime::FileModificationTime); // TODO: On linux check result of this.

    file[JsonKeys::File::IsFrozen] = false;
    file[JsonKeys::File::UserFilePath] = userFilePath;

    bool isUpdated = fsm->updateFileEntity(file);

    if(!isUpdated)
        return false;

    return true;
}

// TODO: Convert this function to accept symbolFilePath only. This means, it will only do the freezing sequence.
//       When converting, also make this function to delete latest copy of the file.
bool FileStorageService::freezeFile(const QString &symbolFilePath, bool isFrozen)
{
    auto fsm = FileStorageManager::instance();
    QJsonObject entity = fsm->getFileJsonBySymbolPath(symbolFilePath);
    QJsonObject parent = fsm->getFolderJsonBySymbolPath(entity[JsonKeys::File::SymbolFolderPath].toString());

    if(!parent[JsonKeys::IsExist].toBool())
        return false;

    if(parent[JsonKeys::Folder::IsFrozen].toBool() && !isFrozen) // Don't activate child of a frozen folder.
        return false;

    entity[JsonKeys::File::IsFrozen] = isFrozen;

    bool result = fsm->updateFileEntity(entity);

    if(result)
        _lastSymbolFilePath = symbolFilePath;

    return result;
}

bool FileStorageService::deleteFile(const QString &symbolFilePath)
{
    auto fsm = FileStorageManager::instance();

    QJsonObject dto = fsm->getFileJsonBySymbolPath(symbolFilePath);
    bool isDeletedFromDb = fsm->deleteFile(symbolFilePath);

    if(!isDeletedFromDb)
        return false;

    _lastSymbolFilePath = dto[JsonKeys::File::SymbolFilePath].toString();

    bool isFrozen = dto[JsonKeys::File::IsFrozen].toBool();

    if(isFrozen)
        return true;

    QString userPath = dto[JsonKeys::File::UserFilePath].toString();

    QFile file(userPath);

    if(file.exists())
        file.remove(); // TODO: Also add delete operations result in the result.

    return true;
}

QString FileStorageService::lastSymbolFolderPath() const
{
    return _lastSymbolFolderPath;
}

QString FileStorageService::lastSymbolFilePath() const
{
    return _lastSymbolFilePath;
}
