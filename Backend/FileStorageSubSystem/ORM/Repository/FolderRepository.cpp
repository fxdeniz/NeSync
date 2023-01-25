#include "FolderRepository.h"

#include <QSqlQuery>
#include <QSqlRecord>

FolderRepository::FolderRepository(const QSqlDatabase &db)
{
    database = db;
    database.open();
}

FolderRepository::~FolderRepository()
{
    database.close();
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
                FolderEntity child;
                child.parentFolderPath = record.value("parent_folder_path").toString();
                child.suffixPath = record.value("suffix_path").toString();
                child.userFolderPath = record.value("user_folder_path").toString();
                child.isFrozen = record.value("is_frozen").toBool();

                result.childFolders.append(child);
            }
        }
    }

    return result;
}

bool FolderRepository::save(const FolderEntity &entity, QSqlError *error)
{
    bool result = false;

    QSqlQuery query(database);
    QString queryTemplate = " INSERT INTO FolderEntity (parent_folder_path, suffix_path, user_folder_path, is_frozen)"
                            " VALUES(:1, :2, :3, :4);" ;

    query.prepare(queryTemplate);

    { // Bind values
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
    }

    query.exec();

    if(error != nullptr)
        error = new QSqlError(query.lastError());

    if(query.lastError().type() == QSqlError::ErrorType::NoError)
        result = true;

    return result;
}
