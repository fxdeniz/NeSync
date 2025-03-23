#include "FileStorageService.h"

#include "JsonDtoFormat.h"
#include "FileStorageSubSystem/FileStorageManager.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>

FileStorageService::FileStorageService(QObject *parent)
    : QObject{parent}
{}

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

bool FileStorageService::deleteFile(const QString &symbolFilePath)
{
    auto fsm = FileStorageManager::instance();

    QJsonObject dto = fsm->getFileJsonBySymbolPath(symbolFilePath);
    bool isDeletedFromDb = fsm->deleteFile(symbolFilePath);

    if(!isDeletedFromDb)
        return false;

    _lastSymbolFilePath = dto[JsonKeys::File::SymbolFilePath].toString();

    QString userPath = dto[JsonKeys::File::UserFilePath].toString();

    QFile file(userPath);

    // Delete when the file is exists. Even when the frozen file is exists.
    if(file.exists())
        file.remove(); // TODO: Also add delete operations result in the result.

    return true;
}

QString FileStorageService::lastSymbolFilePath() const
{
    return _lastSymbolFilePath;
}
