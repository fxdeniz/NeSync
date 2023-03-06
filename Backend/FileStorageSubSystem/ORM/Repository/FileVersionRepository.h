#ifndef FILEVERSIONREPOSITORY_H
#define FILEVERSIONREPOSITORY_H

#include "Entity/FileVersionEntity.h"

#include <QSqlError>
#include <QSqlDatabase>

class FileVersionRepository
{
public:
    FileVersionRepository(const QSqlDatabase &db);
    ~FileVersionRepository();

    FileVersionEntity findVersion(const QString &symbolFilePath, qlonglong versionNumber) const;
    QList<FileVersionEntity> findAllVersions(const QString &symbolFilePath) const;
    qlonglong maxVersionNumber(const QString &symbolFilePath) const;
    bool save(FileVersionEntity &entity, QSqlError *error = nullptr);
    bool deleteEntity(FileVersionEntity &entity, QSqlError *error = nullptr);

private:
    QSqlDatabase database;
};

#endif // FILEVERSIONREPOSITORY_H
