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

bool FileStorageService::relocateFolder(const QString &symbolFolderPath, const QString &destinationUserPath)
{
    auto fsm = FileStorageManager::instance();
    QDir destination(destinationUserPath);

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
    bool isCreatingFirst = true;

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

        if(isCreatingFirst)
        {
            folder[JsonKeys::Folder::UserFolderPath] = destinationUserPath;
            isCreatingFirst = false;
        }
        else
        {
            QJsonObject parent = fsm->getFolderJsonBySymbolPath(folder[JsonKeys::Folder::ParentFolderPath].toString());
            QString path = parent[JsonKeys::Folder::UserFolderPath].toString();
            path += folder[JsonKeys::Folder::SuffixPath].toString();
            folder[JsonKeys::Folder::UserFolderPath] = path;
        }

        bool isFolderUpdated = fsm->updateFileEntity(folder);

        if(!isFolderUpdated)
            return false;

        tree.removeFirst();
    }

    return true;
}

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
