#ifndef FILEREPOSITORY_H
#define FILEREPOSITORY_H

#include "Entity/FileEntity.h"

#include <QSqlError>
#include <QSqlDatabase>

class FileRepository
{
public:
    FileRepository(const QSqlDatabase &db);
    ~FileRepository();

    FileEntity findBySymbolPath(const QString &symbolFilePath, bool includeVersions = false) const;
    QList<FileEntity> findActiveFiles() const;
    bool save(FileEntity &entity, QSqlError *error = nullptr);
    bool deleteEntity(FileEntity &entity, QSqlError *error = nullptr);

private:
    QSqlDatabase database;
};

#endif // FILEREPOSITORY_H
