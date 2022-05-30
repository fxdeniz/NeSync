#ifndef ROWFOLDERRECORD_H
#define ROWFOLDERRECORD_H

#include "TableInfoFolderRecord.h"
#include "TableInfoFileRecord.h"
#include "BaseRow.h"

class RowFolderRecord : public BaseRow
{
public:
    friend class QueryFolderRecord;

    RowFolderRecord();

    QString getDirectory() const;
    const QString &getParentDirectory() const;
    const QString &getSuffixDirectory() const;
    bool getIsFavorite() const;
    PtrTo_RowFolderRecord getChildFolderBySuffix(const QString &suffixDir) const;
    QList<PtrTo_RowFileRecord> getAllChildRowFileRecords() const;
    QList<PtrTo_RowFolderRecord> getAllChildRowFolderRecords() const;
    QList<PtrTo_RowFileRecord> getChildRowFileRecordsByMatchingName(const QString &searchTerm) const;
    QList<PtrTo_RowFileRecord> getChildRowFileRecordsByMatchingExtension(const QString &searchTerm) const;

    QString toString() const override;

    bool setParentDirectory(const QString &newParentDirectory);
    bool setSuffixDirectory(const QString &newSuffixDirectory);
    bool setIsFavorite(bool newIsFavorite);
    bool addChildFolder(const QString &suffixDirectory);

private:
    RowFolderRecord(const QSqlDatabase &db, const QSqlRecord &record);

    template<typename TypeColumnValue>
    void queryTemplateUpdateColumnValue(const QString &columnName,
                                        TypeColumnValue newColumnValue);


    QString parentDirectory;
    QString suffixDirectory;
    bool isFavorite;
};

#endif // ROWFOLDERRECORD_H
