#ifndef QUERYFOLDERRECORD_H
#define QUERYFOLDERRECORD_H

#include "BaseSqlPrimitive.h"
#include "RowFolderRecord.h"

class QueryFolderRecord : public BaseSqlPrimitive
{
public:
    QueryFolderRecord(const QSqlDatabase &db);

    PtrTo_RowFolderRecord selectRowByID(qlonglong folderID) const;
    PtrTo_RowFolderRecord selectRowByDirectory(const QString &directory) const;

    QList<PtrTo_RowFolderRecord> selectRowsByParentDirectory(const QString &parentDirectory) const;
    QList<PtrTo_RowFolderRecord> selectRowsByMatchingDirectory(const QString &searchTerm) const;
    QList<PtrTo_RowFolderRecord> selectChildRowsByMatchingDirectory(const QString &searchTerm, const QString &parentDirectory) const;
    QList<PtrTo_RowFolderRecord> selectFavoriteFolderList() const;
private:
    template <typename T>
    PtrTo_RowFolderRecord queryTemplateSelectRowByKey(const QString &keyColumnName, T keyValue) const;
};

#endif // QUERYFOLDERRECORD_H
