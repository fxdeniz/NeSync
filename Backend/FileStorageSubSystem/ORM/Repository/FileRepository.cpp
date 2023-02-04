#include "FileRepository.h"

#include "FileVersionRepository.h"

#include <QSqlQuery>
#include <QSqlRecord>

FileRepository::FileRepository(const QSqlDatabase &db)
{
    database = db;

    if(!database.isOpen())
        database.open();
}

FileRepository::~FileRepository()
{

}

FileEntity FileRepository::findBySymbolPath(const QString &symbolFilePath, bool includeVersions) const
{
    FileEntity result;

    QSqlQuery query(database);
    QString queryTemplate = "SELECT * FROM FileEntity WHERE symbol_file_path = :1;" ;

    query.prepare(queryTemplate);
    query.bindValue(":1", symbolFilePath);
    query.exec();

    if(query.next())
    {
        QSqlRecord record = query.record();

        result.setIsExist(true);
        result.setPrimaryKey(record.value("symbol_file_path").toString());
        result.fileName = record.value("file_name").toString();
        result.symbolFolderPath = record.value("symbol_folder_path").toString();
        result.isFrozen = record.value("is_frozen").toBool();
    }

    if(result.isExist() && includeVersions)
        result.versionList = FileVersionRepository(database).findAllVersions(symbolFilePath);

    return result;
}

QList<FileEntity> FileRepository::findActiveFiles() const
{
    QList<FileEntity> result;

    QSqlQuery query(database);
    QString queryTemplate = " SELECT * FROM FileEntity NATURAL JOIN FolderEntity"
                            " WHERE user_folder_path IS NOT NULL AND is_frozen IS FALSE;" ;

    query.prepare(queryTemplate);
    query.exec();

    while(query.next())
    {
        QSqlRecord record = query.record();
        FileEntity entity;

        entity.setIsExist(true);
        entity.setPrimaryKey(record.value("symbol_file_path").toString());
        entity.fileName = record.value("file_name").toString();
        entity.symbolFolderPath = record.value("symbol_folder_path").toString();
        entity.isFrozen = record.value("is_frozen").toBool();

        result.append(entity);
    }

    return result;
}

bool FileRepository::save(FileEntity &entity, QSqlError *error)
{
    bool result = false;
    bool isExist = findBySymbolPath(entity.getPrimaryKey()).isExist();

    QSqlQuery query(database);
    QString queryTemplate;

    if(isExist)
    {
        queryTemplate  = " UPDATE FileEntity"
                         " SET symbol_folder_path = :1, file_name = :2, is_frozen = :3"
                         " WHERE symbol_file_path = :4;" ;
    }
    else
    {
        queryTemplate = " INSERT INTO FileEntity (symbol_folder_path, file_name, is_frozen) "
                        " VALUES (:1, :2, :3);" ;
    }

    query.prepare(queryTemplate);

    query.bindValue(":1", entity.symbolFolderPath);
    query.bindValue(":2", entity.fileName);
    query.bindValue(":3", entity.isFrozen);

    if(isExist)
        query.bindValue(":4", entity.getPrimaryKey());

    query.exec();

    if(error != nullptr)
        error = new QSqlError(query.lastError());

    if(query.lastError().type() == QSqlError::ErrorType::NoError)
    {
        result = true;
        entity.setIsExist(true);
        entity.setPrimaryKey(entity.symbolFilePath());
    }

    return result;
}

bool FileRepository::deleteEntity(FileEntity &entity, QSqlError *error)
{
    bool result = false;

    QSqlQuery query(database);
    QString queryTemplate = "DELETE FROM FileEntity WHERE symbol_file_path = :1;" ;

    query.prepare(queryTemplate);
    query.bindValue(":1", entity.getPrimaryKey());
    query.exec();

    if(error != nullptr)
        error = new QSqlError(query.lastError());

    if(query.lastError().type() == QSqlError::ErrorType::NoError)
    {
        entity.setIsExist(false);
        result = true;
    }

    return result;
}
