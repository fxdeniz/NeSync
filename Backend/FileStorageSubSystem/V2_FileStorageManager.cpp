#include "V2_FileStorageManager.h"

#include <QDir>

V2_FileStorageManager::V2_FileStorageManager(const QSqlDatabase &db)
{
    database = db;

    if(!database.isOpen())
        database.open();

    folderRepository = new FolderRepository(database);
}

V2_FileStorageManager::~V2_FileStorageManager()
{
    database.close();

    delete folderRepository;
}

bool V2_FileStorageManager::addNewFolder(const QString &parentSymbolFolderPath, const QString &suffixSymbolFolderPath, const QString &userFolderPath)
{
    bool result = false;

    QString _parentSymbolFolderPath = QDir::fromNativeSeparators(parentSymbolFolderPath);
    QString _suffixSymbolFolderPath = suffixSymbolFolderPath;
    QString _userFolderPath = QDir::toNativeSeparators(userFolderPath);

    bool isUserFolderExist = QDir(userFolderPath).exists(userFolderPath);
    if(!isUserFolderExist)
        return false;

    if(!_userFolderPath.endsWith(QDir::separator()))
        _userFolderPath.append(QDir::separator());

    if(!_parentSymbolFolderPath.startsWith(separator))
        _parentSymbolFolderPath.prepend(separator);

    if(!_parentSymbolFolderPath.endsWith(separator))
        _parentSymbolFolderPath.append(separator);

    if(_suffixSymbolFolderPath.startsWith(separator))
        _suffixSymbolFolderPath.remove(0, 1); // Remove first character

    if(!_suffixSymbolFolderPath.endsWith(separator))
        _suffixSymbolFolderPath.append(separator);

    QString symbolFolerPath = _parentSymbolFolderPath + _suffixSymbolFolderPath;
    FolderEntity entityBySymbolPath = folderRepository->findBySymbolPath(symbolFolerPath);

    QString symbolPathFromUserPath = folderRepository->findSymbolPathByUserFolderPath(_userFolderPath);
    FolderEntity entityByUserPath = folderRepository->findBySymbolPath(symbolPathFromUserPath);

    if(!entityBySymbolPath.isExist() && !entityByUserPath.isExist())
    {
        FolderEntity entity;
        entity.parentFolderPath = _parentSymbolFolderPath;
        entity.suffixPath = _suffixSymbolFolderPath;
        entity.userFolderPath = _userFolderPath;
        entity.isFrozen = false;

        result = folderRepository->save(entity);
    }

    return result;
}
