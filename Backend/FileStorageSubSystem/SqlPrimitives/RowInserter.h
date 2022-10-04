#ifndef ROWINSERTER_H
#define ROWINSERTER_H

#include "TableInfoSaveGroupItem.h"
#include "TableInfoFolderRecord.h"
#include "TableInfoFileVersion.h"
#include "TableInfoFileRecord.h"
#include "TableInfoFileEvent.h"
#include "BaseSqlPrimitive.h"


class I_InsertFolderRecord : virtual public BaseSqlPrimitive
{
public:
    virtual PtrTo_RowFolderRecord insertRootFolder(const QString &rootDir) = 0;

    // TODO Remove when V2_DialogAddNewFolder compeleted
    virtual PtrTo_RowFolderRecord insertChildFolder(const QString &parentDir,
                                                    const QString &suffixDir) = 0;

    virtual PtrTo_RowFolderRecord insertChildFolder(const QString &parentDir,
                                                    const QString &suffixDir,
                                                    const QString &userDir) = 0;
};


class I_InsertFileRecord : virtual public BaseSqlPrimitive
{
public:
    virtual PtrTo_RowFileRecord insertActiveFile(const QString &fileName,
                                                 const QString &fileExtension,
                                                 const QString &symbolDiectory,
                                                 const QString &userDirectory,
                                                 bool isAutoSyncEnabled) = 0;

    virtual PtrTo_RowFileRecord insertFrozenFile(const QString &fileName,
                                                 const QString &fileExtension,
                                                 const QString &symbolDiectory,
                                                 bool isAutoSyncEnabled,
                                                 const QString &userDirectory = "") = 0;
};


class I_InsertFileVersion : virtual public BaseSqlPrimitive
{
public:
    virtual PtrTo_RowFileVersion insertFileVersion(qlonglong parentRecordID,
                                                   qlonglong versionNumber,
                                                   const QString &internalFileName,
                                                   qlonglong size,
                                                   const QString &hash,
                                                   const QString &description = "",
                                                   const QDateTime &timestamp = QDateTime::currentDateTime()) = 0;
};


class I_InsertFileEvent : virtual public BaseSqlPrimitive
{
public:
    virtual PtrTo_RowFileEvent insertUnRegisteredFileEvent(const QString &internalFileName) = 0;
};


class I_InsertSaveGroupItem : virtual public BaseSqlPrimitive
{
public:
    virtual PtrTo_RowSaveGroupItem insertSaveGroupItem(qlonglong saveGroupID,
                                                       qlonglong versionID,
                                                       const QString &originalDescription = "") = 0;
};


class RowInserter : public I_InsertFolderRecord,
                    public I_InsertFileRecord,
                    public I_InsertFileVersion,
                    public I_InsertFileEvent,
                    public I_InsertSaveGroupItem
{
public:
    RowInserter(const QSqlDatabase &db);

    //I_InsertFolderRecord
    PtrTo_RowFolderRecord insertRootFolder(const QString &rootDir) override;

    // TODO Remove when V2_DialogAddNewFolder compeleted
    PtrTo_RowFolderRecord insertChildFolder(const QString &parentDir,
                                            const QString &suffixDir) override;

    PtrTo_RowFolderRecord insertChildFolder(const QString &parentDir,
                                            const QString &suffixDir,
                                            const QString &userDir) override;

    //I_InsertFileRecord
    PtrTo_RowFileRecord insertActiveFile(const QString &fileName,
                                         const QString &fileExtension,
                                         const QString &symbolDiectory,
                                         const QString &userDirectory,
                                         bool isAutoSyncEnabled) override;

    PtrTo_RowFileRecord insertFrozenFile(const QString &fileName,
                                         const QString &fileExtension,
                                         const QString &symbolDiectory,
                                         bool isAutoSyncEnabled,
                                         const QString &userDirectory = "") override;

    //I_InsertFileVersion
    PtrTo_RowFileVersion insertFileVersion(qlonglong parentRecordID,
                                           qlonglong versionNumber,
                                           const QString &internalFileName,
                                           qlonglong size,
                                           const QString &hash,
                                           const QString &description = "",
                                           const QDateTime &timestamp = QDateTime::currentDateTime()) override;

    //I_InsertFileEvent
    PtrTo_RowFileEvent insertUnRegisteredFileEvent(const QString &internalFileName) override;

    //I_InsertSaveGroupItem
    PtrTo_RowSaveGroupItem insertSaveGroupItem(qlonglong saveGroupID,
                                               qlonglong versionID,
                                               const QString &originalDescription = "") override;
};

typedef QScopedPointer<I_InsertFolderRecord> ScopedPtrTo_RowFolderRecordInserter;
typedef QScopedPointer<I_InsertFileRecord> ScopedPtrTo_RowFileRecordInserter;
typedef QScopedPointer<I_InsertFileVersion> ScopedPtrTo_RowFileVersionInserter;
typedef QScopedPointer<I_InsertFileEvent> ScopedPtrTo_RowFileEventInserter;
typedef QScopedPointer<I_InsertSaveGroupItem> ScopedPtrTo_RowSaveGroupItemInserter;

#endif // ROWINSERTER_H
