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
    const QString &getUserDirectory() const;
    bool getIsFavorite() const;
    PtrTo_RowFolderRecord getChildFolderBySuffix(const QString &suffixDir) const;
    QList<PtrTo_RowFileRecord> getAllChildRowFileRecords() const;
    QList<PtrTo_RowFolderRecord> getAllChildRowFolderRecords() const;
    QList<PtrTo_RowFileRecord> getChildRowFileRecordsByMatchingName(const QString &searchTerm) const;
    QList<PtrTo_RowFileRecord> getChildRowFileRecordsByMatchingExtension(const QString &searchTerm) const;

    QString toString() const override;

    bool setParentDirectory(const QString &newParentDirectory);
    bool setSuffixDirectory(const QString &newSuffixDirectory);
    bool setUserDirectory(const QString &newUserDirectory);
    bool setIsFavorite(bool newIsFavorite);

    // TODO Remove when V2_DialogAddNewFolder compeleted
    bool addChildFolder(const QString &suffixDirectory);

    bool addChildFolder(const QString &suffixDirectory, const QString &userDirectory);

private:
    RowFolderRecord(const QSqlDatabase &db, const QSqlRecord &record);

    template<typename TypeColumnValue>
    void queryTemplateUpdateColumnValue(const QString &columnName,
                                        TypeColumnValue newColumnValue);


    QString parentDirectory;
    QString suffixDirectory;
    QString userDirectory;
    bool isFavorite;
};

#endif // ROWFOLDERRECORD_H
