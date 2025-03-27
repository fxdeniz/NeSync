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
