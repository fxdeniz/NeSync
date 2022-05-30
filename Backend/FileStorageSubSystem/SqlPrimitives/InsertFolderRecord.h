#ifndef INSERTFOLDERRECORD_H
#define INSERTFOLDERRECORD_H

#include "BaseSqlPrimitive.h"
#include "RowFolderRecord.h"

class InsertFolderRecord : public BaseSqlPrimitive
{
public:
    InsertFolderRecord(const QSqlDatabase &db);

    PtrTo_RowFolderRecord insertRootFolder(const QString &rootDir);
    PtrTo_RowFolderRecord insertChildFolder(const QString &parentDir, const QString &suffixDir);
};

#endif // INSERTFOLDERRECORD_H
