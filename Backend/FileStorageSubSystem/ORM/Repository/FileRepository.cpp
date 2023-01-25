#include "FileRepository.h"

#include <QSqlQuery>
#include <QSqlRecord>

FileRepository::FileRepository(const QSqlDatabase &db)
{
    database = db;
    database.open();
}

FileRepository::~FileRepository()
{
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
        result.fileName = record.value("file_name").toString();
        result.symbolFolderPath = record.value("symbol_folder_path").toString();
        result.isFrozen = record.value("is_frozen").toBool();
    }

    if(result.isExist() && includeVersions)
    {
        QSqlQuery versionQuery(database);
        QString versionQueryTemplate = "SELECT * FROM FileVersionEntity WHERE symbol_file_path = :1;" ;

        versionQuery.prepare(versionQueryTemplate);
        versionQuery.bindValue(":1", result.symbolFilePath());
        versionQuery.exec();

        while(versionQuery.next())
        {
            QSqlRecord record = versionQuery.record();

            FileVersionEntity currentVersion;
            currentVersion.setIsExist(true);
            currentVersion.symbolFilePath = record.value("symbol_file_path").toString();
            currentVersion.versionNumber = record.value("version_number").toLongLong();
            currentVersion.internalFileName = record.value("internal_file_name").toString();
            currentVersion.size = record.value("size").toLongLong();
            currentVersion.timestamp = record.value("timestamp").toDateTime();
            currentVersion.description = record.value("description").toString();
            currentVersion.hash = record.value("hash").toString();

            result.versionList.append(currentVersion);
        }
    }

    return result;
}

bool FileRepository::save(const FileEntity &entity, QSqlError *error)
{
    bool result = false;

    QSqlQuery query(database);
    QString queryTemplate = " INSERT INTO FileEntity (symbol_folder_path, file_name, is_frozen) "
                            " VALUES (:1, :2, :3);" ;

    query.prepare(queryTemplate);

    query.bindValue(":1", entity.symbolFolderPath);
    query.bindValue(":2", entity.fileName);
    query.bindValue(":3", entity.isFrozen);

    query.exec();

    if(error != nullptr)
        error = new QSqlError(query.lastError());

    if(query.lastError().type() == QSqlError::ErrorType::NoError)
        result = true;

    return result;
}
