#include "V2_FileStorageManager.h"

#include "Utility/JsonDtoFormat.h"
#include "Utility/DatabaseRegistry.h"

#include <QDir>
#include <QUuid>
#include <QJsonArray>
#include <QStandardPaths>
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

QSharedPointer<V2_FileStorageManager> V2_FileStorageManager::instance()
{
    QString tempPath = QStandardPaths::writableLocation(QStandardPaths::StandardLocation::TempLocation);
    tempPath = QDir::toNativeSeparators(tempPath) + QDir::separator();
    QString folderName = "backup_2";
    QDir(tempPath).mkdir(folderName);

    QSqlDatabase storageDb = DatabaseRegistry::fileStorageDatabase();

    auto *rawPtr = new V2_FileStorageManager(storageDb, tempPath + folderName);
    auto result = QSharedPointer<V2_FileStorageManager>(rawPtr);

    return result;
}

V2_FileStorageManager::~V2_FileStorageManager()
{
    database.close();

    delete folderRepository;
    delete fileRepository;
    delete fileVersionRepository;
}

bool V2_FileStorageManager::addNewFolder(const QString &symbolFolderPath, const QString &userFolderPath)
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

bool V2_FileStorageManager::addNewFile(const QString &symbolFolderPath, const QString &pathToFile, bool isFrozen, const QString &description)
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

QJsonObject V2_FileStorageManager::getFolderJsonBySymbolPath(const QString &symbolFolderPath, bool includeChildren) const
{
    QJsonObject result;

    FolderEntity entity = folderRepository->findBySymbolPath(symbolFolderPath, includeChildren);
    result = folderEntityToJsonObject(entity);

    return result;
}

QJsonObject V2_FileStorageManager::getFolderJsonByUserPath(const QString &symbolFolderPath, bool includeChildren) const
{
    QString symbolPath = folderRepository->findSymbolPathByUserFolderPath(symbolFolderPath);
    QJsonObject result = getFolderJsonBySymbolPath(symbolPath, includeChildren);
    return result;
}

QJsonObject V2_FileStorageManager::getFileJson(const QString &symbolFilePath, bool includeVersions) const
{
    QJsonObject result;

    FileEntity entity = fileRepository->findBySymbolPath(symbolFilePath, includeVersions);
    result = fileEntityToJsonObject(entity);

    return result;
}

QJsonObject V2_FileStorageManager::getFileVersionJson(const QString &symbolFilePath, qlonglong versionNumber) const
{
    QJsonObject result;

    FileVersionEntity entity = fileVersionRepository->findVersion(symbolFilePath, versionNumber);
    result = fileVersionEntityToJsonObject(entity);

    return result;
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

QJsonObject V2_FileStorageManager::folderEntityToJsonObject(const FolderEntity &entity) const
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

QJsonObject V2_FileStorageManager::fileEntityToJsonObject(const FileEntity &entity) const
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

QJsonObject V2_FileStorageManager::fileVersionEntityToJsonObject(const FileVersionEntity &entity) const
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
