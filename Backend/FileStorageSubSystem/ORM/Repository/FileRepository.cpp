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
    if(database.isOpen())
        database.close();
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
