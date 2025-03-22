#include "FileStorageService.h"

#include "JsonDtoFormat.h"
#include "FileStorageSubSystem/FileStorageManager.h"

#include <QFile>

FileStorageService::FileStorageService(QObject *parent)
    : QObject{parent}
{}

bool FileStorageService::renameFile(const QString &symbolFilePath, const QString &fileName)
{
    auto fsm = FileStorageManager::instance();

    QJsonObject dto = fsm->getFileJsonBySymbolPath(symbolFilePath);
    bool isFrozen = dto[JsonKeys::File::IsFrozen].toBool();

    if(isFrozen)
        return false;

    dto[JsonKeys::File::FileName] = fileName;
    bool isRenamedInDb = fsm->updateFileEntity(dto);

    if(!isRenamedInDb)
        return false;

    QString userPath = dto[JsonKeys::File::UserFilePath].toString();
    QFile file(userPath);

    bool result = file.rename(fileName); // TODO: Synchronize result of this operation with the db using transactions.
    return result;
}

bool FileStorageService::deleteFile(const QString &symbolFilePath)
{
    auto fsm = FileStorageManager::instance();

    QJsonObject dto = fsm->getFileJsonBySymbolPath(symbolFilePath);
    bool isDeletedFromDb = fsm->deleteFile(symbolFilePath);

    if(!isDeletedFromDb)
        return false;

    QString userPath = dto[JsonKeys::File::UserFilePath].toString();

    QFile file(userPath);

    // Delete when the file is exists. Even when the frozen file is exists.
    if(file.exists())
        file.remove(); // TODO: Also add delete operations result in the result.

    return true;
}
