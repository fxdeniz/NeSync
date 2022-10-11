#include "QueryFolderRecord.h"

#include <QSqlRecord>
#include <QSqlQuery>
#include <QDir>

QueryFolderRecord::QueryFolderRecord(const QSqlDatabase &db) : BaseSqlPrimitive(db, TABLE_NAME_FOLDER_RECORD)
{
}

PtrTo_RowFolderRecord QueryFolderRecord::selectRowByID(qlonglong folderID) const
{
    auto result = this->queryTemplateSelectRowByKey<qlonglong>(TABLE_FOLDER_RECORD_COLNAME_DIRECTORY,  // 1
                                                               folderID);                              // 2

    return result;
}

PtrTo_RowFolderRecord QueryFolderRecord::selectRowByDirectory(const QString &directory) const
{
    auto result = this->queryTemplateSelectRowByKey<QString>(TABLE_FOLDER_RECORD_COLNAME_DIRECTORY, // 1
                                                             directory);                            // 2

    return result;
}

PtrTo_RowFolderRecord QueryFolderRecord::selectRowByUserDirectory(const QString &userDirectory) const
{
    auto result = this->queryTemplateSelectRowByKey<QString>(TABLE_FOLDER_RECORD_COLNAME_USER_DIRECTORY, // 1
                                                             userDirectory);                             // 2

    return result;
}

QList<PtrTo_RowFolderRecord> QueryFolderRecord::selectRowsByParentDirectory(const QString &parentDirectory) const
{
    QList<PtrTo_RowFolderRecord> result;

    QString queryTemplate = "SELECT * FROM %1 WHERE %2 = :2;" ;
    queryTemplate = queryTemplate.arg(this->getTableName(),                           // 1
                                      TABLE_FOLDER_RECORD_COLNAME_PARENT_DIRECTORY);  // 2

    QSqlQuery query(this->getDb());

    query.prepare(queryTemplate);

    query.bindValue(":2", parentDirectory);
    query.exec();

    while(query.next())
    {
        RowFolderRecord *ptr = new RowFolderRecord(this->getDb(), query.record());
        PtrTo_RowFolderRecord item(ptr);
        result.append(item);
    }

    return result;
}

QList<PtrTo_RowFolderRecord> QueryFolderRecord::selectRowsByMatchingDirectory(const QString &searchTerm) const
{
    QList<PtrTo_RowFolderRecord> result;

    QString queryTemplate = "SELECT * FROM %1 WHERE %2 LIKE %:2%;" ;
    queryTemplate = queryTemplate.arg(this->getTableName(),                    // 1
                                      TABLE_FOLDER_RECORD_COLNAME_DIRECTORY);  // 2

    QSqlQuery query(this->getDb());

    query.prepare(queryTemplate);

    query.bindValue(":2", searchTerm);
    query.exec();

    while(query.next())
    {
        RowFolderRecord *ptr = new RowFolderRecord(this->getDb(), query.record());
        PtrTo_RowFolderRecord item (ptr);
        result.append(item);
    }

    return result;
}

QList<PtrTo_RowFolderRecord> QueryFolderRecord::selectChildRowsByMatchingDirectory(const QString &searchTerm, const QString &parentDirectory) const
{
    QList<PtrTo_RowFolderRecord> result;

    QString queryTemplate = "SELECT * FROM %1 WHERE %2 = :2 AND %3 LIKE %:3%;" ;
    queryTemplate = queryTemplate.arg(this->getTableName(),                           // 1
                                      TABLE_FOLDER_RECORD_COLNAME_PARENT_DIRECTORY,   // 2
                                      TABLE_FOLDER_RECORD_COLNAME_DIRECTORY);         // 3

    QSqlQuery query(this->getDb());

    query.prepare(queryTemplate);

    query.bindValue(":2", parentDirectory);
    query.bindValue(":3", searchTerm);
    query.exec();

    while(query.next())
    {
        RowFolderRecord *ptr = new RowFolderRecord(this->getDb(), query.record());
        PtrTo_RowFolderRecord item(ptr);
        result.append(item);
    }

    return result;
}

QList<PtrTo_RowFolderRecord> QueryFolderRecord::selectFavoriteFolderList() const
{
    QList<PtrTo_RowFolderRecord> result;

    QString queryTemplate = "SELECT * FROM %1 WHERE %2 = :2;" ;
    queryTemplate = queryTemplate.arg(this->getTableName(), TABLE_FOLDER_RECORD_COLNAME_IS_FAVORITE);

    QSqlQuery query(queryTemplate);
    query.prepare(queryTemplate);

    query.bindValue(":2", true);
    query.exec();

    while(query.next())
    {
        auto *ptr = new RowFolderRecord(this->getDb(), query.record());
        PtrTo_RowFolderRecord item(ptr);
        result.append(item);
    }

    return result;
}

template<typename T>
PtrTo_RowFolderRecord QueryFolderRecord::queryTemplateSelectRowByKey(const QString &keyColumnName,
                                                                     T keyValue) const
{
    QString queryTemplate = "SELECT * FROM %1 WHERE %2 = :2;" ;
    queryTemplate = queryTemplate.arg(this->getTableName(),   // 1
                                      keyColumnName);         // 2

    QSqlQuery query(this->getDb());
    query.prepare(queryTemplate);

    query.bindValue(":2", keyValue);
    query.exec();

    RowFolderRecord *ptr = nullptr;

    if(!query.next())
        ptr = new RowFolderRecord();
    else
        ptr = new RowFolderRecord(this->getDb(), query.record());

    PtrTo_RowFolderRecord result(ptr);

    return result;
}
