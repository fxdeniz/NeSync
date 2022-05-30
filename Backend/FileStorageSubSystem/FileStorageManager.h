#ifndef FILESTORAGEMANAGER_H
#define FILESTORAGEMANAGER_H

#include "InMemoryDataTypes/SaveGroupItemMetaData.h"
#include "InMemoryDataTypes/FileVersionMetaData.h"
#include "SqlPrimitives/RowFolderRecord.h"
#include "InMemoryDataTypes/FolderMetaData.h"
#include "SqlPrimitives/RowFileRecord.h"
#include "SqlPrimitives/RowFileEvent.h"
#include "InMemoryDataTypes/FileMetaData.h"

#include <QSqlDatabase>

class FileStorageManager : public QObject
{
    Q_OBJECT
public:
    static const QString DB_FILE_NAME;
    static const QString INTERNAL_FILE_NAME_EXTENSION;
    static const QString CONST_SYMBOL_DIRECTORY_SEPARATOR;

    explicit FileStorageManager(const QString &backupDirectory, const QString &symbolDirectory, QObject *parent = nullptr);

    bool addNewFile(const QString &pathToFile,
                    const QString &symbolDirectory,
                    bool isFrozenFile,
                    bool isAutoSyncEnabled,
                    const QString &userDirectory,
                    const QString &description = "",
                    const QString &newFileName = "");

    bool addNewVersion(const QString &pathToSourceFile,
                       const QString &pathToSymbolFile,
                       qlonglong versionNumber,
                       const QString &description = "");

    bool appendNewVersion(const QString &pathToSourceFile,
                          const QString &pathToSymbolFile,
                          const QString &description = "");

    bool deleteFiles(const QStringList &symbolFilePathList);
    bool deleteVersions(const QString &pathToSymbolFile, QList<qlonglong> versionNumberList);

    bool updateNameOfFile(const QString &pathToSymbolFile, const QString &newName);
    bool updateExtensionOfFile(const QString &pathToSymbolFile, const QString &newExtension);
    bool updateSymbolDirectoryOfFile(const QString &pathToSymbolFile, const QString &newSymbolDir);
    bool updateUserDirectoryOfFile(const QString &pathToSymbolFile, const QString &newUserDir);
    bool updateFrozenStatusOfFile(const QString &pathToSymbolFile, bool isFrozen);
    bool updateVersionNumberOfFile(const QString &pathToSymbolFile, qlonglong oldNumber, qlonglong newNumber);
    bool updateContentOfFile(const QString &pathToSymbolFile, qlonglong versionNumber, const QString &pathToNewContent);
    bool markFileAsFavorite(const QString &pathToSymbolFile, bool status);
    void incrementSaveGroupNumber();

    QStringList getMonitoredFilePathList() const;
    qlonglong getCurrentSaveGroupNumber() const;
    QList<qlonglong> getAvailableSaveGroupNumbers() const;
    QList<SaveGroupItemMetaData> getSaveGroupItems(qlonglong saveGropuNumber) const;
    FolderMetaData getFolderMetaData(const QString &directory) const;
    QList<FolderMetaData> getFavoriteFolderMetaDataList() const;
    FileMetaData getFileMetaData(const QString &pathToSymbolFile) const;
    QList<FileMetaData> getFavoriteFileMetaDataList() const;
    FileVersionMetaData getFileVersionMetaData(const QString &pathToSymbolFile, qlonglong versionNumber) const;
    SaveGroupItemMetaData getSaveGroupItemMetaData(const QString &pathToSymbolFile, qlonglong versionNumber) const;

    bool addNewFolder(const QString &directory);
    bool markFolderAsFavorite(const QString &directory, bool status);
    bool isFolderExist(const QString &directory) const;
    bool deleteFolder(const QString &directory);
    bool renameFolder(const QString &directory, const QString &newSuffix);

    const QString &getBackupDirectory() const;
    const QString &getSymbolDirectory() const;

    virtual ~FileStorageManager();

signals:
    void signalMonitoredFileAddedByBackend(const QString &userFilePath);
    void signalMonitoredFileRemovedByBackend(const QString &userFilePath);
    void signalMonitoredFilePathChangedByBackend(const QString &oldUserFilePath, const QString &newUserFilePath);

private:
    qlonglong currentSaveGroupNumber;
    QString backupDirectory;
    QString symbolDirectory;
    QSqlDatabase db;

    PtrTo_RowFolderRecord getRootFolderSymbol() const;
    PtrTo_RowFileEvent addUnRegisteredFile(const QString &pathToSourceFile,
                                     QString *resultInternalFileName,
                                     QString *resultFileHash,
                                     qlonglong *resultSize);

    bool deleteUnRegisteredFile(PtrTo_RowFileEvent event);

    bool transactionInsertRowFileRecord(const QString &fileName,
                                        const QString &fileExtension,
                                        const QString &symbolDirectory,
                                        const QString &userDirectory,
                                        bool isFrozen,
                                        bool isAutoSyncEnabled,
                                        qlonglong fileSize,
                                        const QString &fileHash,
                                        const QString &description,
                                        const QString &internalFileName,
                                        PtrTo_RowFileEvent unRegisteredFileEvent);

    bool transactionInsertRowFileVersion(const QString &pathToSymbolFile,
                                         qlonglong versionNumber,
                                         const QString &internalFileName,
                                         qlonglong fileSize,
                                         const QString &fileHash,
                                         const QString &description,
                                         PtrTo_RowFileEvent unRegisteredFileEvent);

    bool transactionUpdateRowFileVersion(const QString &pathToSymbolFile,
                                         qlonglong versionNumber,
                                         const QString &internalFileName,
                                         const QString &newHash,
                                         qlonglong fileSize,
                                         PtrTo_RowFileEvent rowUnRegisteredFileEvent);

    QList<PtrTo_RowFileEvent> transactionDeleteRowFileRecord(QList<PtrTo_RowFileRecord> targetRecords);
    QList<PtrTo_RowFileEvent> transactionDeleteRowFileVersion(QList<PtrTo_RowFileVersion> targetVersions);
    QString getHashOf(const QByteArray &input) const;
    QString generateInternalFileName() const;
    void extractSqliteDBIfNotExist();
};

#endif // FILESTORAGEMANAGER_H
