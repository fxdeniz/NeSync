#ifndef QUERYFILEVERSION_H
#define QUERYFILEVERSION_H

#include "BaseSqlPrimitive.h"
#include "TableInfoFileVersion.h"
#include "RowFileVersion.h"

class QueryFileVersion : public BaseSqlPrimitive
{
public:
    QueryFileVersion(const QSqlDatabase &db);

    PtrTo_RowFileVersion selectRowByID(qlonglong versionID) const;
    PtrTo_RowFileVersion selectRowByCompositeKey(qlonglong parentRecordID, qlonglong versionNumber) const;
    QList<PtrTo_RowFileVersion> selectAllVersionsOfParentRecord(qlonglong parentRecordID) const;
    qlonglong selectRowFileVersionCount(qlonglong parentRecordID) const;
    QList<qlonglong> selectVersionNumbersOfParentRecord(qlonglong parentRecordID) const;
    qlonglong selectLatestVersionNumber(qlonglong parentRecordID) const;
    QList<PtrTo_RowFileVersion> selectRowsByMatchingDescription(const QString &searchTerm) const;

private:
    template <typename T>
    PtrTo_RowFileVersion queryTemplateSelectRowByKey(const QString &keyColumnName, T keyValue) const;
};

#endif // QUERYFILEVERSION_H
