#include "FileStorageService.h"

#include "JsonDtoFormat.h"
#include "FileStorageSubSystem/FileStorageManager.h"

#include <QFile>

FileStorageService::FileStorageService(QObject *parent)
    : QObject{parent}
{}

bool FileStorageService::deleteFile(const QString &symbolFilePath)
{
    auto fsm = FileStorageManager::instance();

    QJsonObject json = fsm->getFileJsonBySymbolPath(symbolFilePath);
    bool isDeletedFromDb = fsm->deleteFile(symbolFilePath);

    if(!isDeletedFromDb)
        return false;

    QString userPath = json[JsonKeys::File::UserFilePath].toString();

    QFile file(userPath);

    // Delete when the file is exists. Even when the frozen file is exists.
    if(file.exists())
        file.remove(); // TODO: Also add delete operations result in the result.

    return true;
}
