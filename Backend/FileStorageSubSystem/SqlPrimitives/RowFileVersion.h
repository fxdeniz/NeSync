#ifndef ROWFILEVERSION_H
#define ROWFILEVERSION_H

#include "BaseRow.h"
#include "TableInfoFileVersion.h"
#include "TableInfoFileRecord.h"

#include "RowFileEvent.h"
#include "RowSaveGroupItem.h"

#include <QDateTime>

class RowFileVersion : public BaseRow
{
public:
    friend class QueryFileVersion;

    RowFileVersion();

    qlonglong getParentRecordID() const;
    qlonglong getVersionNumber() const;
    const QString &getInternalFileName() const;
    qlonglong getSize() const;
    const QDateTime &getTimestamp() const;
    const QString &getDescription() const;
    const QString &getHash() const;
    PtrTo_RowFileRecord getParentRowFileRecord() const;
    PtrTo_RowSaveGroupItem getSaveGroupItem() const;

    virtual QString toString() const override;

    bool setVersionNumber(qlonglong newVersionNumber);
    bool setInternalFileName(const QString &newInternaFilelName);
    bool setSize(qlonglong newSize);
    bool setTimestamp(const QDateTime &newTimestamp);
    bool setDescription(const QString &newDescription);
    bool setHash(const QString &newHash);
    PtrTo_RowFileEvent markAsUnRegistered();
    PtrTo_RowSaveGroupItem includeInSaveGroup(qlonglong saveGroupID);

private:
    RowFileVersion(const QSqlDatabase &db, const QSqlRecord &record);

    qlonglong parentRecordID;
    qlonglong versionNumber;
    QString internalFileName;
    qlonglong size;
    QDateTime timestamp;
    QString description;
    QString hash;
    bool isCommitted;

    template<typename TypeColumnValue>
    void queryTemplateUpdateColumnValue(const QString &columnName,
                                        TypeColumnValue newColumnValue);

};

#endif // ROWFILEVERSION_H
