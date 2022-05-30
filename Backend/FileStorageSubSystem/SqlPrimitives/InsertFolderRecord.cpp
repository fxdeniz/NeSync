#include "InsertFolderRecord.h"

#include "QueryFolderRecord.h"

#include <QSqlQuery>

InsertFolderRecord::InsertFolderRecord(const QSqlDatabase &db) : BaseSqlPrimitive(db, TABLE_NAME_FOLDER_RECORD)
{
}

PtrTo_RowFolderRecord InsertFolderRecord::insertRootFolder(const QString &rootDir)
{
    QSqlQuery query(this->getTableName());
    QString queryTemplate = "INSERT INTO %1 (%2, %3) "
                            "VALUES(NULL, :3);" ;

    queryTemplate = queryTemplate.arg(this->getTableName(),                           // 1

                                      TABLE_FOLDER_RECORD_COLNAME_PARENT_DIRECTORY,   // 2
                                      TABLE_FOLDER_RECORD_COLNAME_SUFFIX_DIRECTORY);  // 3

    query.prepare(queryTemplate);
    query.bindValue(":3", rootDir);
    query.exec();

    this->setLastError(query.lastError());

    QueryFolderRecord queries(this->getDb());
    PtrTo_RowFolderRecord result = queries.getRowByDirectory(rootDir);

    return result;
}

PtrTo_RowFolderRecord InsertFolderRecord::insertChildFolder(const QString &parentDir, const QString &suffixDir)
{
    QSqlQuery query(this->getTableName());
    QString queryTemplate = "INSERT INTO %1 (%2, %3) "
                            "VALUES(:2, :3);" ;

    queryTemplate = queryTemplate.arg(this->getTableName(),                           // 1

                                      TABLE_FOLDER_RECORD_COLNAME_PARENT_DIRECTORY,   // 2
                                      TABLE_FOLDER_RECORD_COLNAME_SUFFIX_DIRECTORY);  // 3

    query.prepare(queryTemplate);
    query.bindValue(":2", parentDir);
    query.bindValue(":3", suffixDir);
    query.exec();

    this->setLastError(query.lastError());

    QueryFolderRecord queries(this->getDb());
    PtrTo_RowFolderRecord result = queries.getRowByDirectory(parentDir + suffixDir);

    return result;
}
