#include "FileVersionRepository.h"

#include <QSqlQuery>
#include <QSqlRecord>

FileVersionRepository::FileVersionRepository(const QSqlDatabase &db)
{
    database = db;

    if(!database.isOpen())
        database.open();
}

FileVersionRepository::~FileVersionRepository()
{

}

FileVersionEntity FileVersionRepository::findVersion(const QString &symbolFilePath, qlonglong versionNumber) const
{
    FileVersionEntity result;

    QSqlQuery query(database);
    QString queryTemplate = "SELECT * FROM FileVersionEntity WHERE symbol_file_path = :1 AND version_number = :2;" ;

    query.prepare(queryTemplate);
    query.bindValue(":1", symbolFilePath);
    query.bindValue(":2", versionNumber);
    query.exec();

    bool hasNext = query.next();

    if(hasNext)
    {
        QSqlRecord record = query.record();
        result.setIsExist(true);
        result.symbolFilePath = record.value("symbol_file_path").toString();
        result.versionNumber = record.value("version_number").toLongLong();
        result.setPrimaryKey(result.symbolFilePath, result.versionNumber);
        result.internalFileName = record.value("internal_file_name").toString();
        result.size = record.value("size").toLongLong();
        result.lastModifiedTimestamp = record.value("last_modified_timestamp").toDateTime();
        result.description = record.value("description").toString();
        result.hash = record.value("hash").toString();
    }

    return result;
}

QList<FileVersionEntity> FileVersionRepository::findAllVersions(const QString &symbolFilePath) const
{
    QList<FileVersionEntity> result;

    QSqlQuery query(database);
    QString queryTemplate = " SELECT * FROM FileVersionEntity WHERE symbol_file_path = :1"
                            " ORDER BY version_number ASC;" ;

    query.prepare(queryTemplate);
    query.bindValue(":1", symbolFilePath);
    query.exec();

    while(query.next())
    {
        FileVersionEntity entity;
        QSqlRecord record = query.record();
        entity.setIsExist(true);
        entity.symbolFilePath = record.value("symbol_file_path").toString();
        entity.versionNumber = record.value("version_number").toLongLong();
        entity.setPrimaryKey(entity.symbolFilePath, entity.versionNumber);
        entity.internalFileName = record.value("internal_file_name").toString();
        entity.size = record.value("size").toLongLong();
        entity.lastModifiedTimestamp = record.value("last_modified_timestamp").toDateTime();
        entity.description = record.value("description").toString();
        entity.hash = record.value("hash").toString();

        result.append(entity);
    }

    return result;
}

qlonglong FileVersionRepository::maxVersionNumber(const QString &symbolFilePath) const
{
    qlonglong result = -1;

    QString resultColumnName = "result_column";
    QSqlQuery query(database);
    QString queryTemplate = " SELECT MAX(version_number) AS %1"
                            " FROM FileVersionEntity"
                            " WHERE symbol_file_path = :1;" ;

    queryTemplate = queryTemplate.arg(resultColumnName);

    query.prepare(queryTemplate);
    query.bindValue(":1", symbolFilePath);
    query.exec();

    if(query.next())
    {
        QSqlRecord record = query.record();
        result = record.value(resultColumnName).toLongLong();
    }

    return result;
}

bool FileVersionRepository::save(FileVersionEntity &entity, QSqlError *error)
{
    bool result = false;
    bool isExist = findVersion(entity.getPrimaryKey().first,
                               entity.getPrimaryKey().second).isExist();

    QSqlQuery query(database);
    QString queryTemplate;

    if(isExist)
    {
        queryTemplate = " UPDATE FileVersionEntity "
                        " SET symbol_file_path = :1,"
                        "     version_number = :2,"
                        "     internal_file_name = :3,"
                        "     size = :4,"
                        "     last_modified_timestamp = :5,"
                        "     description = :6,"
                        "     hash = :7 "
                        " WHERE symbol_file_path = :8 AND version_number = :9;" ;
    }
    else
    {
        queryTemplate = " INSERT INTO FileVersionEntity (symbol_file_path, "
                        "                                version_number,"
                        "                                internal_file_name,"
                        "                                size,"
                        "                                last_modified_timestamp,"
                        "                                description,"
                        "                                hash)"
                        " VALUES (:1, :2, :3, :4, :5, :6, :7);" ;
    }

    query.prepare(queryTemplate);

    query.bindValue(":1", entity.symbolFilePath);
    query.bindValue(":2", entity.versionNumber);
    query.bindValue(":3", entity.internalFileName);
    query.bindValue(":4", entity.size);

    if(entity.lastModifiedTimestamp.isValid())
        query.bindValue(":5", entity.lastModifiedTimestamp);
    else
        query.bindValue(":5", QDateTime::currentDateTime());

    if(entity.description.isEmpty())
        query.bindValue(":6", QVariant());
    else
        query.bindValue(":6", entity.description);

    if(entity.hash.isEmpty())
        query.bindValue(":7", QVariant());
    else
        query.bindValue(":7", entity.hash);

    if(isExist)
    {
        query.bindValue(":8", entity.getPrimaryKey().first);
        query.bindValue(":9", entity.getPrimaryKey().second);
    }

    query.exec();

    if(error != nullptr)
        error = new QSqlError(query.lastError());

    if(query.lastError().type() == QSqlError::ErrorType::NoError)
    {
        result = true;
        entity.setIsExist(true);
        entity.setPrimaryKey(entity.symbolFilePath, entity.versionNumber);
    }

    return result;
}

bool FileVersionRepository::deleteEntity(FileVersionEntity &entity, QSqlError *error)
{
    bool result = false;

    QSqlQuery query(database);
    QString queryTemplate = " DELETE FROM FileVersionEntity"
                            " WHERE symbol_file_path = :1 AND version_number = :2;" ;

    query.prepare(queryTemplate);
    query.bindValue(":1", entity.getPrimaryKey().first);
    query.bindValue(":2", entity.getPrimaryKey().second);
    query.exec();

    if(error != nullptr)
        error = new QSqlError(query.lastError());

    if(query.numRowsAffected() == 1 && query.lastError().type() == QSqlError::ErrorType::NoError)
    {
        entity.setIsExist(false);
        result = true;
    }

    return result;
}
