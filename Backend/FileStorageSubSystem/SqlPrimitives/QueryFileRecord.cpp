#include "QueryFileRecord.h"

#include <QSqlQuery>

QueryFileRecord::QueryFileRecord(const QSqlDatabase &db) : BaseSqlPrimitive(db, TABLE_NAME_FILE_RECORD)
{
}

bool QueryFileRecord::isRowExistByUserFilePath(const QString &userFilePath) const
{
    if(this->selectRowByUserFilePath(userFilePath)->isExistInDB())
        return true;

    return false;
}

bool QueryFileRecord::isRowExistBySymbolFilePath(const QString &symbolFilePath) const
{
    if(this->selectRowBySymbolFilePath(symbolFilePath)->isExistInDB())
        return true;

    return false;
}

bool QueryFileRecord::isRowExistByRecordID(qlonglong recordID) const
{
    if(this->selectRowByID(recordID)->isExistInDB())
        return true;

    return false;
}

PtrTo_RowFileRecord QueryFileRecord::selectRowByUserFilePath(const QString &userFilePath) const
{
     auto result = this->queryTemplateSelectRowByKey<QString>(TABLE_FILE_RECORD_COLNAME_USER_FILE_PATH, userFilePath);

    return result;
}

PtrTo_RowFileRecord QueryFileRecord::selectRowBySymbolFilePath(const QString &symbolFilePath) const
{
    auto result = this->queryTemplateSelectRowByKey<QString>(TABLE_FILE_RECORD_COLNAME_SYMBOL_FILE_PATH, symbolFilePath);

    return result;
}

PtrTo_RowFileRecord QueryFileRecord::selectRowByID(qlonglong recordID) const
{
    auto result = this->queryTemplateSelectRowByKey<qlonglong>(TABLE_FILE_RECORD_COLNAME_RECORD_ID, recordID);

    return result;
}

QStringList QueryFileRecord::selectUserFilePathListFromActiveFiles() const
{
    QStringList result;

    QString columnName = "result_column";
    QString queryTemplate = "SELECT %1 AS %2 FROM %3 WHERE %4 = :4;" ;

    queryTemplate = queryTemplate.arg(TABLE_FILE_RECORD_COLNAME_USER_FILE_PATH,  // 1
                                      columnName,                                // 2
                                      this->getTableName(),                      // 3
                                      TABLE_FILE_RECORD_COLNAME_IS_FROZEN);      // 4

    QSqlQuery query(this->getDb());
    query.prepare(queryTemplate);

    query.bindValue(":4", false);
    query.exec();

    while(query.next())
    {
        QString item = query.record().value(columnName).toString();
        result.append(item);
    }

    return result;
}

QStringList QueryFileRecord::selectUserFolderPathListFromActiveFiles() const
{
    QStringList result;

    QString columnName = "result_column";
    QString queryTemplate = "SELECT DISTINCT %1 AS %2 FROM %3 WHERE %4 = :4;" ;

    queryTemplate = queryTemplate.arg(TABLE_FILE_RECORD_COLNAME_USER_DIRECTORY,  // 1
                                      columnName,                                // 2
                                      this->getTableName(),                      // 3
                                      TABLE_FILE_RECORD_COLNAME_IS_FROZEN);      // 4

    QSqlQuery query(this->getDb());
    query.prepare(queryTemplate);

    query.bindValue(":4", false);
    query.exec();

    while(query.next())
    {
        QString item = query.record().value(columnName).toString();
        result.append(item);
    }

    return result;
}

QStringList QueryFileRecord::selectUserFolderPathListFromAllFiles() const
{
    QStringList result;

    QString columnName = "result_column";
    QString queryTemplate = "SELECT DISTINCT %1 AS %2 FROM %3 WHERE %1 IS NOT NULL;" ;

    queryTemplate = queryTemplate.arg(TABLE_FILE_RECORD_COLNAME_USER_DIRECTORY,  // 1
                                      columnName,                                // 2
                                      this->getTableName());                     // 3

    QSqlQuery query(this->getDb());
    query.prepare(queryTemplate);

    query.exec();

    while(query.next())
    {
        QString item = query.record().value(columnName).toString();
        result.append(item);
    }

    return result;
}

QList<PtrTo_RowFileRecord> QueryFileRecord::selectFavoriteFileList() const
{
    QList< PtrTo_RowFileRecord> result;

    QString queryTemplate = "SELECT * FROM %1 WHERE %2 = :2;" ;
    queryTemplate = queryTemplate.arg(this->getTableName(), TABLE_FILE_RECORD_COLNAME_IS_FAVORITE);

    QSqlQuery query(this->getDb());

    query.prepare(queryTemplate);

    query.bindValue(":2", true);
    query.exec();

    while(query.next())
    {
        auto *ptr = new RowFileRecord(this->getDb(), query.record());
        PtrTo_RowFileRecord item(ptr);
        result.append(item);
    }

    return result;
}

QList<PtrTo_RowFileRecord> QueryFileRecord::selectRowsBySymbolDirectory(const QString &symbolDirectory) const
{
    QList<PtrTo_RowFileRecord> result;

    QString queryTemplate = "SELECT * FROM %1 WHERE %2 = :2;" ;
    queryTemplate = queryTemplate.arg(this->getTableName(), TABLE_FILE_RECORD_COLNAME_SYMBOL_DIRECTORY);

    QSqlQuery query(this->getDb());

    query.prepare(queryTemplate);

    query.bindValue(":2", symbolDirectory);
    query.exec();

    while(query.next())
    {
        auto ptr = new RowFileRecord(this->getDb(), query.record());
        PtrTo_RowFileRecord item(ptr);
        result.append(item);
    }

    return result;
}

QList<PtrTo_RowFileRecord> QueryFileRecord::selectRowsByMatchingFileName(const QString &searchTerm) const
{
    QList<PtrTo_RowFileRecord> result;

    QString queryTemplate = "SELECT * FROM %1 WHERE %2 LIKE %:2%;" ;
    queryTemplate = queryTemplate.arg(this->getTableName(), TABLE_FILE_RECORD_COLNAME_FILE_NAME);

    QSqlQuery query(this->getDb());

    query.prepare(queryTemplate);

    query.bindValue(":2", searchTerm);
    query.exec();

    while(query.next())
    {
        RowFileRecord *ptr = new RowFileRecord(this->getDb(), query.record());
        PtrTo_RowFileRecord item(ptr);
        result.append(item);
    }

    return result;
}

QList<PtrTo_RowFileRecord> QueryFileRecord::selectRowsByMatchingFileExtension(const QString &searchTerm) const
{
    QList<PtrTo_RowFileRecord> result;

    QString queryTemplate = "SELECT * FROM %1 WHERE %2 LIKE %:2%;" ;
    queryTemplate = queryTemplate.arg(this->getTableName(), TABLE_FILE_RECORD_COLNAME_FILE_EXTENSION);

    QSqlQuery query(this->getDb());

    query.prepare(queryTemplate);

    query.bindValue(":2", searchTerm);
    query.exec();

    while(query.next())
    {
        RowFileRecord *ptr = new RowFileRecord(this->getDb(), query.record());
        PtrTo_RowFileRecord item(ptr);
        result.append(item);
    }

    return result;
}

QList<PtrTo_RowFileRecord> QueryFileRecord::selectRowsByMatchingFileNameFromDirectory(const QString &searchTerm, const QString &directory) const
{
    QList<PtrTo_RowFileRecord> result;

    QString queryTemplate = "SELECT * FROM %1 WHERE %2 = :2 AND %3 LIKE %:3%;" ;
    queryTemplate = queryTemplate.arg(this->getTableName(),                        // 1
                                      TABLE_FILE_RECORD_COLNAME_SYMBOL_DIRECTORY,  // 2
                                      TABLE_FILE_RECORD_COLNAME_FILE_NAME);        // 3

    QSqlQuery query(this->getDb());

    query.prepare(queryTemplate);

    query.bindValue(":2", directory);
    query.bindValue(":3", searchTerm);
    query.exec();

    while(query.next())
    {
        RowFileRecord *ptr = new RowFileRecord(this->getDb(), query.record());
        PtrTo_RowFileRecord item(ptr);
        result.append(item);
    }

    return result;
}

QList<PtrTo_RowFileRecord> QueryFileRecord::selectRowsByMatchingFilExtensionFromDirectory(const QString &searchTerm, const QString &directory) const
{
    QList<PtrTo_RowFileRecord> result;

    QString queryTemplate = "SELECT * FROM %1 WHERE %2 = :2 AND %3 LIKE %:3%;" ;
    queryTemplate = queryTemplate.arg(this->getTableName(),                        // 1
                                      TABLE_FILE_RECORD_COLNAME_SYMBOL_DIRECTORY,  // 2
                                      TABLE_FILE_RECORD_COLNAME_FILE_EXTENSION);        // 3

    QSqlQuery query(this->getDb());

    query.prepare(queryTemplate);

    query.bindValue(":2", directory);
    query.bindValue(":3", searchTerm);
    query.exec();

    while(query.next())
    {
        RowFileRecord *ptr = new RowFileRecord(this->getDb(), query.record());
        PtrTo_RowFileRecord item(ptr);
        result.append(item);
    }

    return result;
}

template<typename T>
PtrTo_RowFileRecord QueryFileRecord::queryTemplateSelectRowByKey(const QString &keyColumnName, T keyValue) const
{
    QString queryTemplate = "SELECT * FROM %1 WHERE %2 = :2;" ;
    queryTemplate = queryTemplate.arg(this->getTableName(),   // 1

                                      keyColumnName);         // 2

    QSqlQuery query(this->getDb());
    query.prepare(queryTemplate);

    query.bindValue(":2", keyValue);

    query.exec();

    RowFileRecord *ptr = nullptr;

    if(!query.next())
       ptr = new RowFileRecord();
    else
       ptr = new RowFileRecord(this->getDb(), query.record());

    PtrTo_RowFileRecord result = PtrTo_RowFileRecord(ptr);

    return result;
}
