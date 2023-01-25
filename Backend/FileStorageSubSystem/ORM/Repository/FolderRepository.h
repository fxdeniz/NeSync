#ifndef FOLDERREPOSITORY_H
#define FOLDERREPOSITORY_H

#include <QSqlError>
#include <QSqlDatabase>

#include "Entity/FolderEntity.h"

class FolderRepository
{
public:
    FolderRepository(const QSqlDatabase &db);
    ~FolderRepository();

    FolderEntity findBySymbolPath(const QString &symbolFolderPath) const;
    bool save(const FolderEntity &entity, QSqlError *error = nullptr);

private:
    QSqlDatabase database;
};

#endif // FOLDERREPOSITORY_H
