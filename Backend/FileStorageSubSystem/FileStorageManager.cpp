#include "FileStorageManager.h"

#include "Utility/JsonDtoFormat.h"
#include "Utility/DatabaseRegistry.h"

#include <QDir>
#include <QUuid>
#include <QJsonArray>
#include <QStandardPaths>
#include <QCryptographicHash>

FileStorageManager::FileStorageManager(const QSqlDatabase &db, const QString &backupFolderPath)
{
    setBackupFolderPath(backupFolderPath);
    database = db;

    if(!database.isOpen())
        database.open();

    folderRepository = new FolderRepository(database);
    fileRepository = new FileRepository(database);
    fileVersionRepository = new FileVersionRepository(database);
}

QSharedPointer<FileStorageManager> FileStorageManager::instance()
{
    QString tempPath = QStandardPaths::writableLocation(QStandardPaths::StandardLocation::TempLocation);
    tempPath = QDir::toNativeSeparators(tempPath) + QDir::separator();
    QString folderName = "backup_2";
    QDir(tempPath).mkdir(folderName);

    QSqlDatabase storageDb = DatabaseRegistry::fileStorageDatabase();

    auto *rawPtr = new FileStorageManager(storageDb, tempPath + folderName);
    auto result = QSharedPointer<FileStorageManager>(rawPtr);

    return result;
}

FileStorageManager::~FileStorageManager()
{
    database.close();

    delete folderRepository;
    delete fileRepository;
    delete fileVersionRepository;
}

bool FileStorageManager::addNewFolder(const QString &symbolFolderPath, const QString &userFolderPath)
{
    bool result = false;

    QString _symbolFolderPath = QDir::fromNativeSeparators(symbolFolderPath);
    QString _userFolderPath = QDir::toNativeSeparators(userFolderPath);

    bool isUserFolderExist = QDir(userFolderPath).exists();
    if(!isUserFolderExist)
        return false;

    if(!_userFolderPath.endsWith(QDir::separator()) && !_userFolderPath.isEmpty())
        _userFolderPath.append(QDir::separator());

    if(!_symbolFolderPath.startsWith(separator))
        _symbolFolderPath.prepend(separator);

    if(!_symbolFolderPath.endsWith(separator))
        _symbolFolderPath.append(separator);

    FolderEntity entityBySymbolPath = folderRepository->findBySymbolPath(_symbolFolderPath);

    QString symbolPathFromUserPath = folderRepository->findSymbolPathByUserFolderPath(_userFolderPath);
    FolderEntity entityByUserPath = folderRepository->findBySymbolPath(symbolPathFromUserPath);

    if(!entityBySymbolPath.isExist() && !entityByUserPath.isExist())
    {
        QStringList tokenList = _symbolFolderPath.chopped(1).split(separator); // Remove last seperator.
        for(QString &currentToken : tokenList)
            currentToken.append(separator);

        QString parentSymbolFolderPath = "";
        QString suffixPath = "";

        for(const QString &currentToken : tokenList)
        {
            suffixPath = currentToken;
            QString currentSymbolFolder = parentSymbolFolderPath + suffixPath;
            FolderEntity entity = folderRepository->findBySymbolPath(currentSymbolFolder);

            if(!entity.isExist())
            {
                entity.parentFolderPath = parentSymbolFolderPath;
                entity.suffixPath = suffixPath;
                entity.isFrozen = false;
                entity.userFolderPath = "";

                if(currentSymbolFolder == _symbolFolderPath)
                    entity.userFolderPath = _userFolderPath;

                result = folderRepository->save(entity);
            }

            parentSymbolFolderPath.append(suffixPath);
        }
    }

    return result;
}

bool FileStorageManager::addNewFile(const QString &symbolFolderPath, const QString &pathToFile, bool isFrozen, const QString &description)
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

bool FileStorageManager::appendVersion(const QString &symbolFilePath, const QString &pathToFile, const QString &description)
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

    bool isOpen = file.open(QFile::OpenModeFlag::ReadOnly);

    if(!isOpen)
        return false;

    QCryptographicHash hasher(QCryptographicHash::Algorithm::Sha3_256);
    hasher.addData(&file);
    QString fileHash = QString(hasher.result().toHex());

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

bool FileStorageManager::deleteFile(const QString &symbolFilePath)
{
    bool result = false;
    FileEntity entity = fileRepository->findBySymbolPath(symbolFilePath, true);

    if(entity.isExist())
    {
        QList<FileVersionEntity> fileVersionList = entity.getVersionList();
        QStringList internalPathList;

        for(const FileVersionEntity &version : fileVersionList)
            internalPathList.append(getBackupFolderPath() + version.internalFileName);

        result = fileRepository->deleteEntity(entity);

        if(result == true)
        {
            for(const QString &filePath : internalPathList)
                QFile::remove(filePath);
        }
    }

    return result;
}

bool FileStorageManager::updateFileEntity(QJsonObject fileDto)
{
    bool isFileNameExist = fileDto.contains(JsonKeys::File::FileName);
    bool isSymbolFilePathExist = fileDto.contains(JsonKeys::File::SymbolFilePath);
    bool isSymbolFolderPath = fileDto.contains(JsonKeys::File::SymbolFolderPath);
    bool isFrozenExist = fileDto.contains(JsonKeys::File::IsFrozen);

    if(!isFileNameExist || !isSymbolFilePathExist || !isSymbolFolderPath || !isFrozenExist)
        return false;

    bool isFileNameString = fileDto[JsonKeys::File::FileName].isString();
    bool isSymbolFilePathString = fileDto[JsonKeys::File::SymbolFilePath].isString();
    bool isSymbolFolderPathString = fileDto[JsonKeys::File::SymbolFolderPath].isString();
    bool isFrozenBool = fileDto[JsonKeys::File::IsFrozen].isBool();

    if(!isFileNameString || !isSymbolFilePathString || !isSymbolFolderPathString || !isFrozenBool)
        return false;

    FileEntity entity = fileRepository->findBySymbolPath(fileDto[JsonKeys::File::SymbolFilePath].toString());
    entity.fileName = fileDto[JsonKeys::File::FileName].toString();
    entity.symbolFolderPath = fileDto[JsonKeys::File::SymbolFolderPath].toString();
    entity.isFrozen = fileDto[JsonKeys::File::IsFrozen].toBool();

    bool result = fileRepository->save(entity);

    return result;
}

QJsonObject FileStorageManager::getFolderJsonBySymbolPath(const QString &symbolFolderPath, bool includeChildren) const
{
    QJsonObject result;

    FolderEntity entity = folderRepository->findBySymbolPath(symbolFolderPath, includeChildren);
    result = folderEntityToJsonObject(entity);

    return result;
}

QJsonObject FileStorageManager::getFolderJsonByUserPath(const QString &userFolderPath, bool includeChildren) const
{
    QString symbolPath = folderRepository->findSymbolPathByUserFolderPath(userFolderPath);
    QJsonObject result = getFolderJsonBySymbolPath(symbolPath, includeChildren);
    return result;
}

QJsonObject FileStorageManager::getFileJsonBySymbolPath(const QString &symbolFilePath, bool includeVersions) const
{
    QJsonObject result;

    FileEntity entity = fileRepository->findBySymbolPath(symbolFilePath, includeVersions);
    result = fileEntityToJsonObject(entity);

    return result;
}

QJsonObject FileStorageManager::getFileJsonByUserPath(const QString &userFilePath, bool includeVersions) const
{
    QFileInfo info(userFilePath);
    QString userFolderPath = QDir::toNativeSeparators(info.absolutePath()) + QDir::separator();
    QString symbolFolderPath = folderRepository->findSymbolPathByUserFolderPath(userFolderPath);
    QString symbolFilePath = symbolFolderPath + info.fileName();

    QJsonObject result = getFileJsonBySymbolPath(symbolFilePath, includeVersions);
    return result;
}

QJsonObject FileStorageManager::getFileVersionJson(const QString &symbolFilePath, qlonglong versionNumber) const
{
    QJsonObject result;

    FileVersionEntity entity = fileVersionRepository->findVersion(symbolFilePath, versionNumber);
    result = fileVersionEntityToJsonObject(entity);

    return result;
}

QJsonArray FileStorageManager::getActiveFolderList() const
{
    QJsonArray result;

    QList<FolderEntity> queryResult = folderRepository->findActiveFolders();

    for(const FolderEntity &entity : queryResult)
    {
        QJsonObject folderJson = folderEntityToJsonObject(entity);
        result.append(folderJson);
    }

    return result;
}

QJsonArray FileStorageManager::getActiveFileList() const
{
    QJsonArray result;

    QList<FileEntity> queryResult = fileRepository->findActiveFiles();

    for(const FileEntity &entity : queryResult)
    {
        QJsonObject folderJson = fileEntityToJsonObject(entity);
        result.append(folderJson);
    }

    return result;
}

QString FileStorageManager::getBackupFolderPath() const
{
    return backupFolderPath;
}

void FileStorageManager::setBackupFolderPath(const QString &newBackupFolderPath)
{
    backupFolderPath = QDir::toNativeSeparators(newBackupFolderPath);

    if(!backupFolderPath.endsWith(QDir::separator()))
        backupFolderPath.append(QDir::separator());
}

QString FileStorageManager::generateRandomFileName()
{
    QString result = QUuid::createUuid().toString(QUuid::StringFormat::Id128) + ".file";
    return result;
}

QJsonObject FileStorageManager::folderEntityToJsonObject(const FolderEntity &entity) const
{
    QJsonObject result;

    result[JsonKeys::IsExist] = entity.isExist();
    result[JsonKeys::Folder::ParentFolderPath] = entity.parentFolderPath;
    result[JsonKeys::Folder::SuffixPath] = entity.suffixPath;
    result[JsonKeys::Folder::SymbolFolderPath] = entity.symbolFolderPath();
    result[JsonKeys::Folder::UserFolderPath] = entity.userFolderPath;
    result[JsonKeys::Folder::IsFrozen] = entity.isFrozen;

    result[JsonKeys::Folder::ChildFolders] = QJsonValue();
    result[JsonKeys::Folder::ChildFiles] = QJsonValue();

    if(!entity.getChildFolders().isEmpty())
    {
        QJsonArray jsonArrayChildFolder;
        for(const FolderEntity &entityChildFolder : entity.getChildFolders())
        {
            QJsonObject jsonChildFolder;

            jsonChildFolder[JsonKeys::IsExist] = entityChildFolder.isExist();
            jsonChildFolder[JsonKeys::Folder::ParentFolderPath] = entityChildFolder.parentFolderPath;
            jsonChildFolder[JsonKeys::Folder::SuffixPath] = entityChildFolder.suffixPath;
            jsonChildFolder[JsonKeys::Folder::SymbolFolderPath] = entityChildFolder.symbolFolderPath();
            jsonChildFolder[JsonKeys::Folder::UserFolderPath] = entityChildFolder.userFolderPath;
            jsonChildFolder[JsonKeys::Folder::IsFrozen] = entityChildFolder.isFrozen;

            jsonChildFolder[JsonKeys::Folder::ChildFolders] = QJsonValue();
            jsonChildFolder[JsonKeys::Folder::ChildFiles] = QJsonValue();
            jsonArrayChildFolder.append(jsonChildFolder);
        }

        result[JsonKeys::Folder::ChildFolders] = jsonArrayChildFolder;
    }

    if(!entity.getChildFiles().isEmpty())
    {
        QJsonArray jsonArrayFileList;

        for(const FileEntity &entityFile : entity.getChildFiles())
        {
            QJsonObject jsonChildFile;
            jsonChildFile = fileEntityToJsonObject(entityFile);
            jsonArrayFileList.append(jsonChildFile);
        }

        result[JsonKeys::Folder::ChildFiles] = jsonArrayFileList;
    }

    return result;
}

QJsonObject FileStorageManager::fileEntityToJsonObject(const FileEntity &entity) const
{
    QJsonObject result;


    result[JsonKeys::IsExist] = entity.isExist();
    result[JsonKeys::File::FileName] = entity.fileName;
    result[JsonKeys::File::IsFrozen] = entity.isFrozen;
    result[JsonKeys::File::SymbolFolderPath] = entity.symbolFolderPath;
    result[JsonKeys::File::SymbolFilePath] = entity.symbolFilePath();
    result[JsonKeys::File::UserFilePath] = QJsonValue();
    result[JsonKeys::File::VersionList] = QJsonValue();

    FolderEntity parentEntity = folderRepository->findBySymbolPath(entity.symbolFolderPath);

    if(!parentEntity.userFolderPath.isEmpty())
        result[JsonKeys::File::UserFilePath] = parentEntity.userFolderPath + entity.fileName;

    if(!entity.getVersionList().isEmpty())
    {
        QJsonArray jsonArrayVersionList;
        for(const FileVersionEntity &entityFileVersion : entity.getVersionList())
        {
            QJsonObject jsonFileVersion;
            jsonFileVersion = fileVersionEntityToJsonObject(entityFileVersion);
            jsonArrayVersionList.append(jsonFileVersion);
        }

        result[JsonKeys::File::VersionList] = jsonArrayVersionList;
    }

    return result;
}

QJsonObject FileStorageManager::fileVersionEntityToJsonObject(const FileVersionEntity &entity) const
{
    QJsonObject result;

    result[JsonKeys::IsExist] = entity.isExist();
    result[JsonKeys::FileVersion::SymbolFilePath] = entity.symbolFilePath;
    result[JsonKeys::FileVersion::VersionNumber] = entity.versionNumber;
    result[JsonKeys::FileVersion::Size] = entity.size;
    result[JsonKeys::FileVersion::Timestamp] = entity.timestamp.toString(Qt::DateFormat::TextDate);
    result[JsonKeys::FileVersion::Description] = entity.description;
    result[JsonKeys::FileVersion::Hash] = entity.hash;

    return result;
}
