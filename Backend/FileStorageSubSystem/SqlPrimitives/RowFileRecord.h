#ifndef ROWFILERECORD_H
#define ROWFILERECORD_H

#include "BaseRow.h"
#include "TableInfoFileRecord.h"
#include "RowFileVersion.h"
#include "RowFileEvent.h"

#include <QSharedPointer>
#include <QSqlRecord>

class RowFileRecord : public BaseRow
{
public:
    friend class QueryFileRecord;

    RowFileRecord();

    PtrTo_RowFileVersion insertVersion(qlonglong versionNumber,
                                       const QString &internalFileName,
                                       qlonglong size,
                                       const QString &hash,
                                       const QString &description = "");

    const QString &getFileName() const;
    const QString &getFileExtension() const;
    const QString &getSymbolDirectory() const;
    QString getSymbolFilePath() const;
    const QString &getUserDirectory() const;
    QString getUserFilePath() const;
    bool getIsFavorite() const;
    bool getIsFrozen() const;
    bool getIsAutoSyncEnabled() const;
    qlonglong getRowFileVersionCount() const;
    qlonglong getLatestVersionNumber() const;
    PtrTo_RowFileVersion getRowFileVersion(qlonglong versionNumber) const;
    QList<qlonglong> getVersionNumbers() const;
    QList<PtrTo_RowFileVersion> getAllRowFileVersions() const;

    virtual QString toString() const override;

    bool setFileName(const QString &newFileName);
    bool setFileExtension(const QString &newFileExtension);
    bool setSymbolDirectory(const QString &newSymbolDirectory);
    bool setUserDirectory(const QString &newUserDirectory);
    bool setIsFavorite(bool newIsFavorite);
    bool setIsFrozen(bool newIsFrozen);
    bool setIsAutoSyncEnabled(bool newIsAutoSyncEnabled);

private:
    RowFileRecord(const QSqlDatabase &db, const QSqlRecord &record);

    QString fileName;
    QString fileExtension;
    QString userDirectory;
    QString symbolDirectory;
    bool isFavorite;
    bool isFrozen;
    bool isAutoSyncEnabled;

    template<typename TypeColumnValue>
    void queryTemplateUpdateColumnValue(const QString &columnName,
                                        TypeColumnValue newColumnValue);

};

#endif // ROWFILERECORD_H
