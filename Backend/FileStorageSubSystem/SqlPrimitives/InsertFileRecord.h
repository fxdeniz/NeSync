#ifndef INSERTFILERECORD_H
#define INSERTFILERECORD_H

#include "BaseSqlPrimitive.h"
#include "RowFileRecord.h"

class InsertFileRecord : public BaseSqlPrimitive
{
public:
    InsertFileRecord(const QSqlDatabase &db);

    PtrTo_RowFileRecord insertActiveFileRecord(const QString &fileName,
                                            const QString &fileExtension,
                                            const QString &symbolDiectory,
                                            const QString &userDirectory);

    PtrTo_RowFileRecord insertFrozenFileRecord(const QString &fileName,
                                            const QString &fileExtension,
                                            const QString &symbolDiectory,
                                            const QString &userDirectory = "");

private:
};

#endif // INSERTFILERECORD_H
