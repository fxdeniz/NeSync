#include "FileVersionRepository.h"

#include <QSqlQuery>

FileVersionRepository::FileVersionRepository(const QSqlDatabase &db)
{
    database = db;
    database.open();
}

FileVersionRepository::~FileVersionRepository()
{
    database.close();
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
