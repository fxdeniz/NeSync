#include "FileStorageManager.h"

#include "Utility/AppConfig.h"
#include "Utility/JsonDtoFormat.h"
#include "Utility/DatabaseRegistry.h"

#include <QDir>
#include <QUuid>
#include <QJsonArray>
#include <QStandardPaths>
#include <QCryptographicHash>

FileStorageManager::FileStorageManager(const QSqlDatabase &db, const QString &backupFolderPath)
{
    setStorageFolderPath(backupFolderPath);
    database = db;

    if(!database.isOpen())
        database.open();

    folderRepository = new FolderRepository(database);
    fileRepository = new FileRepository(database);
    fileVersionRepository = new FileVersionRepository(database);
}

QSharedPointer<FileStorageManager> FileStorageManager::instance()
{
    AppConfig config;
    QSqlDatabase storageDb = DatabaseRegistry::fileStorageDatabase();

    auto *rawPtr = new FileStorageManager(storageDb, config.getStorageFolderPath());
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

    bool isUserFolderExist = QDir(_userFolderPath).exists(); // Returns true on empty _userFolderPath
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
                entity.userFolderPath = "";
                entity.isFrozen = true;

                if(isUserFolderExist && !_userFolderPath.isEmpty())
                    entity.isFrozen = false;

                if(currentSymbolFolder == _symbolFolderPath)
                    entity.userFolderPath = _userFolderPath;

                result = folderRepository->save(entity);
            }

            parentSymbolFolderPath.append(suffixPath);
        }
    }

    return result;
}

bool FileStorageManager::addNewFile(const QString &symbolFolderPath,
                                    const QString &pathToFile,
                                    bool isFrozen,
                                    const QString newFileName,
                                    const QString &description)
{
    QString _symbolFolderPath = QDir::fromNativeSeparators(symbolFolderPath);
    QFileInfo info(pathToFile);

    if(!info.isFile() || !info.exists())
        return false;

    if(!_symbolFolderPath.startsWith(separator))
        _symbolFolderPath.prepend(separator);

    if(!_symbolFolderPath.endsWith(separator))
        _symbolFolderPath.append(separator);

    QString _fileName = info.fileName();
    if(!newFileName.isEmpty())
        _fileName = newFileName;

    FolderEntity folderEntity = folderRepository->findBySymbolPath(_symbolFolderPath);

    if(!folderEntity.isExist()) // Symbol folder does not exist
        return false;

    QString symbolFilePath = folderEntity.symbolFolderPath() + _fileName;
    FileEntity fileEntity = fileRepository->findBySymbolPath(symbolFilePath);

    if(fileEntity.isExist()) // Symbol folder is already exist
        return false;

    fileEntity.fileName = _fileName;
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
    QString generatedFilePath = getStorageFolderPath() + internalFileName;
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
    versionEntity.lastModifiedTimestamp = info.lastModified();
    versionEntity.description = description;
    versionEntity.hash = fileHash;

    bool isVersionInserted = fileVersionRepository->save(versionEntity);

    if(!isVersionInserted)
        return false;

    return true;
}

bool FileStorageManager::deleteFolder(const QString &symbolFolderPath)
{
    bool result = false;
    FolderEntity entity = folderRepository->findBySymbolPath(symbolFolderPath);

    if(entity.isExist())
    {
        QList<FileEntity> fileList = fileRepository->findAllChildFiles(symbolFolderPath);
        for(const FileEntity &fileEntity : fileList)
            deleteFile(fileEntity.symbolFilePath());

        result = folderRepository->deleteEntity(entity);
    }

    return result;
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
            internalPathList.append(getStorageFolderPath() + version.internalFileName);

        result = fileRepository->deleteEntity(entity);

        if(result == true)
        {
            for(const QString &filePath : internalPathList)
                QFile::remove(filePath);
        }
    }

    return result;
}

bool FileStorageManager::deleteFileVersion(const QString &symbolFilePath, qlonglong versionNumber)
{
    bool result = false;
    FileVersionEntity entity = fileVersionRepository->findVersion(symbolFilePath, versionNumber);

    if(entity.isExist())
    {
        qlonglong maxVersionNumber = fileVersionRepository->maxVersionNumber(symbolFilePath);
        if(maxVersionNumber <= 1) // Don't delete single version (therefore the entire file)
            return false;

        result = fileVersionRepository->deleteEntity(entity);

        if(result == true)
        {
            QString internalFilePath = getStorageFolderPath() + entity.internalFileName;
            QFile::remove(internalFilePath);

            FileEntity parentEntity = fileRepository->findBySymbolPath(symbolFilePath, true);

            result = sortFileVersionEntities(parentEntity);
        }
    }

    return result;
}

bool FileStorageManager::updateFolderEntity(QJsonObject folderDto, bool updateFrozenStatusOfChildren)
{
    bool isParentFolderPathExist = folderDto.contains(JsonKeys::Folder::ParentFolderPath);
    bool isSuffixPathExist = folderDto.contains(JsonKeys::Folder::SuffixPath);
    bool isSymbolFolderPathExist = folderDto.contains(JsonKeys::Folder::SymbolFolderPath);
    bool isUserFolderPathExist = folderDto.contains(JsonKeys::Folder::UserFolderPath);
    bool isFrozenExist = folderDto.contains(JsonKeys::Folder::IsFrozen);

    if(!isParentFolderPathExist || !isSuffixPathExist || !isSymbolFolderPathExist || !isUserFolderPathExist || !isFrozenExist)
        return false;

    bool isParentFolderPathString = folderDto[JsonKeys::Folder::ParentFolderPath].isString();
    bool isSuffixPathString = folderDto[JsonKeys::Folder::SuffixPath].isString();
    bool isSymbolFolderPathString = folderDto[JsonKeys::Folder::SymbolFolderPath].isString();
    bool isUserFolderPathString = folderDto[JsonKeys::Folder::UserFolderPath].isString();
    bool isFrozenBool = folderDto[JsonKeys::Folder::IsFrozen].isBool();

    if(!isParentFolderPathString || !isSuffixPathString || !isSymbolFolderPathString || !isUserFolderPathString || !isFrozenBool)
        return false;

    FolderEntity entity = folderRepository->findBySymbolPath(folderDto[JsonKeys::Folder::SymbolFolderPath].toString());
    entity.parentFolderPath = folderDto[JsonKeys::Folder::ParentFolderPath].toString();
    entity.suffixPath = folderDto[JsonKeys::Folder::SuffixPath].toString();
    entity.userFolderPath = folderDto[JsonKeys::Folder::UserFolderPath].toString();
    entity.isFrozen = folderDto[JsonKeys::Folder::IsFrozen].toBool();

    if(!entity.parentFolderPath.startsWith(separator))
        entity.parentFolderPath.prepend(separator);

    if(!entity.parentFolderPath.endsWith(separator))
        entity.parentFolderPath.append(separator);

    if(!entity.suffixPath.endsWith(separator))
        entity.suffixPath.append(separator);

    bool result = folderRepository->save(entity);

    if(result == true && updateFrozenStatusOfChildren == true)
    {
        result = folderRepository->setIsFrozenOfChildren(entity.getPrimaryKey(),
                                                         folderDto[JsonKeys::Folder::IsFrozen].toBool());
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

bool FileStorageManager::updateFileVersionEntity(QJsonObject versionDto)
{
    bool isSymbolFilePathExist = versionDto.contains(JsonKeys::FileVersion::SymbolFilePath);
    bool isVersionNumberExist = versionDto.contains(JsonKeys::FileVersion::VersionNumber);

    if(!isSymbolFilePathExist || !isVersionNumberExist)
        return false;

    bool isSymbolFilePathString = versionDto[JsonKeys::FileVersion::SymbolFilePath].isString();
    bool isVersionNumberDouble = versionDto[JsonKeys::FileVersion::VersionNumber].isDouble();

    if(!isSymbolFilePathString || !isVersionNumberDouble)
        return false;

    FileVersionEntity entity = fileVersionRepository->findVersion(versionDto[JsonKeys::FileVersion::SymbolFilePath].toString(),
                                                                  versionDto[JsonKeys::FileVersion::VersionNumber].toInteger());

    entity.description = versionDto[JsonKeys::FileVersion::Description].toString(entity.description);
    entity.versionNumber = versionDto[JsonKeys::FileVersion::NewVersionNumber].toInteger(entity.versionNumber);
    bool result = fileVersionRepository->save(entity);

    return result;
}

bool FileStorageManager::sortFileVersionsInIncreasingOrder(const QString &symbolFilePath)
{
    bool result = false;

    FileEntity parentEntity = fileRepository->findBySymbolPath(symbolFilePath, true);

    if(parentEntity.isExist())
        result = sortFileVersionEntities(parentEntity);

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

QString FileStorageManager::getStorageFolderPath() const
{
    return storageFolderPath;
}

void FileStorageManager::setStorageFolderPath(const QString &newStorageFolderPath)
{
    storageFolderPath = QDir::toNativeSeparators(newStorageFolderPath);

    if(!storageFolderPath.endsWith(QDir::separator()))
        storageFolderPath.append(QDir::separator());
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
    result[JsonKeys::Folder::IsFrozen] = entity.isFrozen;

    result[JsonKeys::Folder::UserFolderPath] = QJsonValue(QJsonValue::Type::Null);
    result[JsonKeys::Folder::ChildFolders] = QJsonValue(QJsonValue::Type::Null);
    result[JsonKeys::Folder::ChildFiles] = QJsonValue(QJsonValue::Type::Null);

    if(!entity.isFrozen)
        result[JsonKeys::Folder::UserFolderPath] = entity.userFolderPath;

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
            jsonChildFolder[JsonKeys::Folder::IsFrozen] = entityChildFolder.isFrozen;

            jsonChildFolder[JsonKeys::Folder::UserFolderPath] = QJsonValue(QJsonValue::Type::Null);
            if(!entityChildFolder.isFrozen)
                jsonChildFolder[JsonKeys::Folder::UserFolderPath] = entityChildFolder.userFolderPath;

            jsonChildFolder[JsonKeys::Folder::ChildFolders] = QJsonValue(QJsonValue::Type::Null);
            jsonChildFolder[JsonKeys::Folder::ChildFiles] = QJsonValue(QJsonValue::Type::Null);
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
    result[JsonKeys::File::MaxVersionNumber] = fileVersionRepository->maxVersionNumber(entity.symbolFilePath());
    result[JsonKeys::File::UserFilePath] = QJsonValue(QJsonValue::Type::Null);
    result[JsonKeys::File::VersionList] = QJsonValue(QJsonValue::Type::Null);

    FolderEntity parentEntity = folderRepository->findBySymbolPath(entity.symbolFolderPath);

    if(!parentEntity.userFolderPath.isEmpty() && !entity.isFrozen)
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
    result[JsonKeys::FileVersion::Timestamp] = entity.lastModifiedTimestamp.toString(Qt::DateFormat::TextDate);
    result[JsonKeys::FileVersion::Description] = entity.description;
    result[JsonKeys::FileVersion::Hash] = entity.hash;
    result[JsonKeys::FileVersion::InternalFileName] = entity.internalFileName;

    result[JsonKeys::FileVersion::NewVersionNumber] = QJsonValue(QJsonValue::Type::Null);

    return result;
}

bool FileStorageManager::sortFileVersionEntities(const FileEntity &parentEntity)
{
    bool result = false;

    if(parentEntity.isExist())
    {
        qlonglong versionNumber = 1;

        for(FileVersionEntity &currentVersion : parentEntity.getVersionList())
        {
            currentVersion.versionNumber = versionNumber;
            result = fileVersionRepository->save(currentVersion);
            ++versionNumber;

            if(result == false)
                return false;
        }
    }

    return result;
}
