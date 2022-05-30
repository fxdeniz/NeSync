#ifndef QUERYFILERECORD_H
#define QUERYFILERECORD_H

#include "BaseSqlPrimitive.h"
#include "TableInfoFileRecord.h"
#include "RowFileRecord.h"

class QueryFileRecord : public BaseSqlPrimitive
{
public:
    QueryFileRecord(const QSqlDatabase &db);

    bool isRowExistByUserFilePath(const QString &userFilePath) const;
    bool isRowExistBySymbolFilePath(const QString &symbolFilePath) const;
    bool isRowExistByRecordID(qlonglong recordID) const;

    PtrTo_RowFileRecord selectRowByUserFilePath(const QString &userFilePath) const;
    PtrTo_RowFileRecord selectRowBySymbolFilePath(const QString &symbolFilePath) const;
    PtrTo_RowFileRecord selectRowByID(qlonglong recordID) const;

    QStringList selectUserFilePathListFromActiveFiles() const;
    QList<PtrTo_RowFileRecord> selectFavoriteFileList() const;
    QList<PtrTo_RowFileRecord> selectRowsBySymbolDirectory(const QString &symbolDirectory) const;
    QList<PtrTo_RowFileRecord> selectRowsByMatchingFileName(const QString &searchTerm) const;
    QList<PtrTo_RowFileRecord> selectRowsByMatchingFileExtension(const QString &searchTerm) const;
    QList<PtrTo_RowFileRecord> selectRowsByMatchingFileNameFromDirectory(const QString &searchTerm, const QString &directory) const;
    QList<PtrTo_RowFileRecord> selectRowsByMatchingFilExtensionFromDirectory(const QString &searchTerm, const QString &directory) const;
private:
    template <typename T>
    PtrTo_RowFileRecord queryTemplateSelectRowByKey(const QString &keyColumnName, T keyValue) const;

};

#endif // QUERYFILERECORD_H
