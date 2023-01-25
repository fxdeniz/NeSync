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
    database.close();
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
        result.internalFileName = record.value("internal_file_name").toString();
        result.size = record.value("size").toLongLong();
        result.timestamp = record.value("timestamp").toDateTime();
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
        entity.internalFileName = record.value("internal_file_name").toString();
        entity.size = record.value("size").toLongLong();
        entity.timestamp = record.value("timestamp").toDateTime();
        entity.description = record.value("description").toString();
        entity.hash = record.value("hash").toString();

        result.append(entity);
    }

    return result;
}

bool FileVersionRepository::save(const FileVersionEntity &entity, QSqlError *error)
{
    bool result = false;

    QSqlQuery query(database);
    QString queryTemplate = " INSERT INTO FileVersionEntity (symbol_file_path, "
                            "                                version_number,"
                            "                                internal_file_name,"
                            "                                size,"
                            "                                timestamp,"
                            "                                description,"
                            "                                hash)"
                            " VALUES (:1, :2, :3, :4, :5, :6, :7);" ;

    query.prepare(queryTemplate);

    query.bindValue(":1", entity.symbolFilePath);
    query.bindValue(":2", entity.versionNumber);
    query.bindValue(":3", entity.internalFileName);
    query.bindValue(":4", entity.size);

    if(entity.timestamp.isValid())
        query.bindValue(":5", entity.timestamp);
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

    query.exec();

    if(error != nullptr)
        error = new QSqlError(query.lastError());

    if(query.lastError().type() == QSqlError::ErrorType::NoError)
        result = true;

    return result;
}
