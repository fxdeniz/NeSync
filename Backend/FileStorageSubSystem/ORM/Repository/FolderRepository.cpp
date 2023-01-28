#include "FolderRepository.h"

#include <QSqlQuery>
#include <QSqlRecord>

FolderRepository::FolderRepository(const QSqlDatabase &db)
{
    database = db;

    if(!database.isOpen())
        database.open();
}

FolderRepository::~FolderRepository()
{

}

FolderEntity FolderRepository::findBySymbolPath(const QString &symbolFolderPath, bool includeChildren) const
{
    FolderEntity result;

    QSqlQuery query(database);
    QString queryTemplate = "SELECT * FROM FolderEntity WHERE symbol_folder_path = :1;" ;

    query.prepare(queryTemplate);
    query.bindValue(":1", symbolFolderPath);
    query.exec();

    if(query.next())
    {
        QSqlRecord record = query.record();

        result.setIsExist(true);
        result.setPrimaryKey(record.value("symbol_folder_path").toString());
        result.parentFolderPath = record.value("parent_folder_path").toString();
        result.suffixPath = record.value("suffix_path").toString();
        result.userFolderPath = record.value("user_folder_path").toString();
        result.isFrozen = record.value("is_frozen").toBool();

        if(result.isExist() && includeChildren)
        {
            QSqlQuery childFolderQuery(database);
            QString childFolderQueryTemplate = "SELECT * FROM FolderEntity WHERE parent_folder_path = :1;" ;

            childFolderQuery.prepare(childFolderQueryTemplate);
            childFolderQuery.bindValue(":1", result.symbolFolderPath());
            childFolderQuery.exec();

            while(childFolderQuery.next())
            {
                QSqlRecord record = childFolderQuery.record();
                FolderEntity childFolder;
                childFolder.setIsExist(true);
                childFolder.setPrimaryKey(record.value("symbol_folder_path").toString());
                childFolder.parentFolderPath = record.value("parent_folder_path").toString();
                childFolder.suffixPath = record.value("suffix_path").toString();
                childFolder.userFolderPath = record.value("user_folder_path").toString();
                childFolder.isFrozen = record.value("is_frozen").toBool();

                result.childFolders.append(childFolder);
            }

            QSqlQuery childFileQuery(database);
            QString childFileQueryTemplate = "SELECT * FROM FileEntity WHERE symbol_folder_path = :1;" ;

            childFileQuery.prepare(childFileQueryTemplate);
            childFileQuery.bindValue(":1", result.symbolFolderPath());
            childFileQuery.exec();

            while(childFileQuery.next())
            {
                QSqlRecord record = childFileQuery.record();
                FileEntity childFile;
                childFile.setIsExist(true);
                childFile.setPrimaryKey(record.value("symbol_file_path").toString());
                childFile.fileName = record.value("file_name").toString();
                childFile.symbolFolderPath = record.value("symbol_folder_path").toString();
                childFile.isFrozen = record.value("is_frozen").toBool();

                result.childFiles.append(childFile);
            }
        }
    }

    return result;
}

QString FolderRepository::findSymbolPathByUserFolderPath(const QString &userFolderPath) const
{
    QString result = "";
    QSqlQuery query(database);
    QString queryTemplate = "SELECT * FROM FolderEntity WHERE user_folder_path = :1;" ;

    query.prepare(queryTemplate);
    query.bindValue(":1", userFolderPath);
    query.exec();

    if(query.next())
    {
        QSqlRecord record = query.record();
        result = record.value("symbol_folder_path").toString();
    }

    return result;
}

QList<FolderEntity> FolderRepository::findActiveFolders() const
{
    QList<FolderEntity> result;

    QSqlQuery query(database);
    QString queryTemplate = " SELECT * FROM FolderEntity"
                            " WHERE user_folder_path IS NOT NULL AND is_frozen IS FALSE;" ;

    query.prepare(queryTemplate);
    query.exec();

    while(query.next())
    {
        QSqlRecord record = query.record();
        FolderEntity entity;

        entity.setIsExist(true);
        entity.setPrimaryKey(record.value("symbol_folder_path").toString());
        entity.parentFolderPath = record.value("parent_folder_path").toString();
        entity.suffixPath = record.value("suffix_path").toString();
        entity.userFolderPath = record.value("user_folder_path").toString();
        entity.isFrozen = record.value("is_frozen").toBool();

        result.append(entity);
    }

    return result;
}

bool FolderRepository::save(FolderEntity &entity, QSqlError *error)
{
    bool result = false;
    bool isExist = findBySymbolPath(entity.getPrimaryKey()).isExist();
    QSqlQuery query(database);
    QString queryTemplate;

    if(isExist)
    {
        queryTemplate = " UPDATE FolderEntity"
                        " SET parent_folder_path = :1, suffix_path = :2, user_folder_path = :3, is_frozen = :4"
                        " WHERE symbol_folder_path = :5;" ;
    }
    else
    {
        queryTemplate = " INSERT INTO FolderEntity (parent_folder_path, suffix_path, user_folder_path, is_frozen)"
                        " VALUES(:1, :2, :3, :4);" ;
    }

    query.prepare(queryTemplate);

    if(entity.parentFolderPath.isEmpty())
        query.bindValue(":1", QVariant());
    else
        query.bindValue(":1", entity.parentFolderPath);

    query.bindValue(":2", entity.suffixPath);

    if(entity.userFolderPath.isEmpty())
        query.bindValue(":3", QVariant());
    else
        query.bindValue(":3", entity.userFolderPath);

    query.bindValue(":4", entity.isFrozen);

    if(isExist)
        query.bindValue(":5", entity.getPrimaryKey());

    query.exec();

    if(error != nullptr)
        error = new QSqlError(query.lastError());

    if(query.lastError().type() == QSqlError::ErrorType::NoError)
    {
        result = true;
        entity.setIsExist(true);
        entity.setPrimaryKey(entity.symbolFolderPath());
    }

    return result;
}
