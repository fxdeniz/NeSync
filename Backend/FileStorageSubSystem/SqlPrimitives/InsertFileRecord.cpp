#include "InsertFileRecord.h"
#include "QueryFileRecord.h"

#include <QSqlQuery>
#include <QSqlRecord>


InsertFileRecord::InsertFileRecord(const QSqlDatabase &db) : BaseSqlPrimitive(db, TABLE_NAME_FILE_RECORD)
{
}

PtrTo_RowFileRecord InsertFileRecord::insertActiveFileRecord(const QString &fileName,
                                                                       const QString &fileExtension,
                                                                       const QString &symbolDiectory,
                                                                       const QString &userDirectory)
{
    QSqlQuery query(this->getDb());
    QString queryTemplate = "INSERT INTO %1 (%2, %3, %4, %5, %6) VALUES (:2, :3, :4, :5, :6);" ;

    queryTemplate = queryTemplate.arg(this->getTableName(),                        // 1

                                      TABLE_FILE_RECORD_COLNAME_FILE_NAME,         // 2
                                      TABLE_FILE_RECORD_COLNAME_FILE_EXTENSION,    // 3
                                      TABLE_FILE_RECORD_COLNAME_SYMBOL_DIRECTORY,  // 4
                                      TABLE_FILE_RECORD_COLNAME_USER_DIRECTORY,    // 5
                                      TABLE_FILE_RECORD_COLNAME_IS_FROZEN);        // 6

    query.prepare(queryTemplate);
    query.bindValue(":2", fileName);
    query.bindValue(":3", fileExtension);
    query.bindValue(":4", symbolDiectory);
    query.bindValue(":5", userDirectory);
    query.bindValue(":6", false);

    query.exec();
    this->setLastError(query.lastError());

    QueryFileRecord queries(this->getDb());
    auto result = queries.getRowBySymbolFilePath(symbolDiectory + fileName + fileExtension);
    return result;
}

PtrTo_RowFileRecord InsertFileRecord::insertFrozenFileRecord(const QString &fileName,
                                                                       const QString &fileExtension,
                                                                       const QString &symbolDiectory,
                                                                       const QString &userDirectory)
{
    QSqlQuery query(this->getDb());
    QString queryTemplate = "INSERT INTO %1 (%2, %3, %4, %5, %6) VALUES (:2, :3, :4, :5, :6);" ;

    if(userDirectory.isEmpty())
        queryTemplate = "INSERT INTO %1 (%2, %3, %4, %5, %6) VALUES (:2, :3, :4, NULL, :6);" ;

    queryTemplate = queryTemplate.arg(this->getTableName(),                        // 1

                                      TABLE_FILE_RECORD_COLNAME_FILE_NAME,         // 2
                                      TABLE_FILE_RECORD_COLNAME_FILE_EXTENSION,    // 3
                                      TABLE_FILE_RECORD_COLNAME_SYMBOL_DIRECTORY,  // 4
                                      TABLE_FILE_RECORD_COLNAME_USER_DIRECTORY,    // 5
                                      TABLE_FILE_RECORD_COLNAME_IS_FROZEN);        // 6

    query.prepare(queryTemplate);
    query.bindValue(":2", fileName);
    query.bindValue(":3", fileExtension);
    query.bindValue(":4", symbolDiectory);

    if(!userDirectory.isEmpty())
        query.bindValue(":5", userDirectory);

    query.bindValue(":6", true);

    query.exec();
    this->setLastError(query.lastError());

    QueryFileRecord queries(this->getDb());
    auto result = queries.getRowBySymbolFilePath(symbolDiectory + fileName + fileExtension);
    return result;
}
