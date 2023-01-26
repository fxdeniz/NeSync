#include "V2_FileStorageManager.h"

#include <QDir>
#include <QUuid>
#include <QCryptographicHash>

V2_FileStorageManager::V2_FileStorageManager(const QSqlDatabase &db, const QString &backupFolderPath)
{
    setBackupFolderPath(backupFolderPath);
    database = db;

    if(!database.isOpen())
        database.open();

    folderRepository = new FolderRepository(database);
    fileRepository = new FileRepository(database);
    fileVersionRepository = new FileVersionRepository(database);
}

V2_FileStorageManager::~V2_FileStorageManager()
{
    database.close();

    delete folderRepository;
    delete fileRepository;
    delete fileVersionRepository;
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

bool V2_FileStorageManager::addNewFile(const QString &symbolFolderPath, const QString &pathToFile, const QString &description, bool isFrozen)
{
    QString _symbolFolderPath = QDir::fromNativeSeparators(symbolFolderPath);
    QFileInfo info(pathToFile);

    if(!info.isFile() || !info.exists())
        return false;

    if(!_symbolFolderPath.startsWith(separator))
        _symbolFolderPath.prepend(separator);

    if(!_symbolFolderPath.endsWith(separator))
        _symbolFolderPath.append(separator);

    FolderEntity folderEntity = folderRepository->findBySymbolPath(_symbolFolderPath);

    if(!folderEntity.isExist()) // Symbol folder does not exist
        return false;

    QString symbolFilePath = folderEntity.symbolFolderPath() + info.fileName();
    FileEntity fileEntity = fileRepository->findBySymbolPath(symbolFilePath);

    if(fileEntity.isExist()) // Symbol folder is already exist
        return false;

    fileEntity.fileName = info.fileName();
    fileEntity.symbolFolderPath = folderEntity.symbolFolderPath();
    fileEntity.isFrozen = isFrozen;

    bool isFileInserted = fileRepository->save(fileEntity);

    if(!isFileInserted)
        return false;

    bool result = appendVersion(symbolFilePath, pathToFile, description);
    return result;
}

bool V2_FileStorageManager::appendVersion(const QString &symbolFilePath, const QString &pathToFile, const QString &description)
{
    QFileInfo info(pathToFile);

    if(!info.isFile() || !info.exists())
        return false;

    FileEntity fileEntity = fileRepository->findBySymbolPath(symbolFilePath);

    if(!fileEntity.isExist())
        return false;

    qlonglong versionNumber = fileVersionRepository->maxVersionNumber(fileEntity.symbolFilePath());

    if(versionNumber <= 0)
        versionNumber = 1;
    else
        versionNumber += 1;

    QFile file(pathToFile);
    QString internalFileName = generateRandomFileName();
    QString generatedFilePath = getBackupFolderPath() + internalFileName;
    bool isCopied = file.copy(generatedFilePath);

    if(!isCopied)
        return false;

    QCryptographicHash hasher(QCryptographicHash::Algorithm::Sha3_256);
    QString fileHash = QString(hasher.result().toHex());
    file.open(QFile::OpenModeFlag::ReadOnly);

    bool isHashed = hasher.addData(&file);
    if(!isHashed)
        return false;

    file.close();

    FileVersionEntity versionEntity;
    versionEntity.symbolFilePath = fileEntity.symbolFilePath();
    versionEntity.versionNumber = versionNumber;
    versionEntity.size = file.size();
    versionEntity.internalFileName = internalFileName;
    versionEntity.timestamp = QDateTime::currentDateTime();
    versionEntity.description = description;
    versionEntity.hash = fileHash;

    bool isVersionInserted = fileVersionRepository->save(versionEntity);

    if(!isVersionInserted)
        return false;

    return true;
}

QString V2_FileStorageManager::getBackupFolderPath() const
{
    return backupFolderPath;
}

void V2_FileStorageManager::setBackupFolderPath(const QString &newBackupFolderPath)
{
    backupFolderPath = QDir::toNativeSeparators(newBackupFolderPath);

    if(!backupFolderPath.endsWith(QDir::separator()))
        backupFolderPath.append(QDir::separator());
}

QString V2_FileStorageManager::generateRandomFileName()
{
    QString result = QUuid::createUuid().toString(QUuid::StringFormat::Id128) + ".file";
    return result;
}
