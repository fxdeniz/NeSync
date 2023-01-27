#ifndef V2_FILESTORAGEMANAGER_H
#define V2_FILESTORAGEMANAGER_H

#include "ORM/Repository/FolderRepository.h"
#include "ORM/Repository/FileRepository.h"
#include "ORM/Repository/FileVersionRepository.h"

#include <QJsonObject>

namespace JsonKeys
{
    const inline QString IsExist = QStringLiteral("isExist");

    namespace Folder
    {
        const inline QString ParentFolderPath = QStringLiteral("parentFolderPath");
        const inline QString SuffixPath = QStringLiteral("suffixPath");
        const inline QString SymbolFolderPath = QStringLiteral("symbolFolderPath");
        const inline QString UserFolderPath = QStringLiteral("userFolderPath");
        const inline QString IsFrozen = QStringLiteral("isFrozen");
        const inline QString ChildFolders = QStringLiteral("childFolders");
        const inline QString ChildFiles = QStringLiteral("childFiles");
    }

    namespace File
    {
        const inline QString FileName = QStringLiteral("fileName");
        const inline QString SymbolFolderPath = QStringLiteral("symbolFolderPath");
        const inline QString SymbolFilePath = QStringLiteral("symbolFilePath");
        const inline QString IsFrozen = QStringLiteral("isFrozen");
        const inline QString VersionList = QStringLiteral("versionList");
    }

    namespace FileVersion
    {
        const inline QString SymbolFilePath = QStringLiteral("symbolFilePath");
        const inline QString VersionNumber = QStringLiteral("versionNumber");
        const inline QString Size = QStringLiteral("size");
        const inline QString Timestamp = QStringLiteral("timestamp");
        const inline QString Description = QStringLiteral("description");
        const inline QString Hash = QStringLiteral("hash");
    }
}

class V2_FileStorageManager
{
private:
    V2_FileStorageManager(const QSqlDatabase &db, const QString &backupFolderPath);

public:
    static const inline QString separator = "/";
    static V2_FileStorageManager instance();

    ~V2_FileStorageManager();

    bool addNewFolder(const QString &parentSymbolFolderPath,
                      const QString &suffixSymbolFolderPath,
                      const QString &userFolderPath);

    bool addNewFile(const QString &symbolFolderPath,
                    const QString &pathToFile,
                    const QString &description = "",
                    bool isFrozen = false);

    bool appendVersion(const QString &symbolFilePath,
                       const QString &pathToFile,
                       const QString &description = "");

    QJsonObject getFolderJsonBySymbolPath(const QString &symbolFolderPath, bool includeChildren = false) const;
    QJsonObject getFolderJsonByUserPath(const QString &symbolFolderPath, bool includeChildren = false) const;
    QJsonObject getFileJson(const QString &symbolFilePath, bool includeVersions) const;
    QJsonObject getFileVersionJson(const QString &symbolFilePath, qlonglong versionNumber) const;

    QString getBackupFolderPath() const;
    void setBackupFolderPath(const QString &newBackupFolderPath);

private:
    QString generateRandomFileName();
    QJsonObject folderEntityToJsonObject(const FolderEntity &entity) const;
    QJsonObject fileEntityToJsonObject(const FileEntity &entity) const;
    QJsonObject fileVersionEntityToJsonObject(const FileVersionEntity &entity) const;

private:
    QString backupFolderPath;
    QSqlDatabase database;
    FolderRepository *folderRepository;
    FileRepository *fileRepository;
    FileVersionRepository *fileVersionRepository;
};

#endif // V2_FILESTORAGEMANAGER_H
