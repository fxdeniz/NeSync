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

    bool save(const FileVersionEntity &entity, QSqlError *error = nullptr);

private:
    QSqlDatabase database;
};

#endif // FILEVERSIONREPOSITORY_H
