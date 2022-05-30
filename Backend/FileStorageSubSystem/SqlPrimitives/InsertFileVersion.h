#ifndef INSERTFILEVERSION_H
#define INSERTFILEVERSION_H

#include "BaseSqlPrimitive.h"
#include "RowFileVersion.h"

#include <QDateTime>

class InsertFileVersion : public BaseSqlPrimitive
{
public:
    InsertFileVersion(const QSqlDatabase &db);

    PtrTo_RowFileVersion insertInto(qlonglong parentRecordID,
                              qlonglong versionNumber,
                              const QString &internalFileName,
                              qlonglong size,
                              const QString &hash,
                              const QString &description = "",
                              const QDateTime &timestamp = QDateTime::currentDateTime());
};

#endif // INSERTFILEVERSION_H
