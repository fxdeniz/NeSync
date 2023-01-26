#ifndef FOLDERREPOSITORY_H
#define FOLDERREPOSITORY_H

#include "Entity/FolderEntity.h"

#include <QSqlError>
#include <QSqlDatabase>

class FolderRepository
{
public:
    FolderRepository(const QSqlDatabase &db);
    ~FolderRepository();

    FolderEntity findBySymbolPath(const QString &symbolFolderPath, bool includeChildren = false) const;
    QString findSymbolPathByUserFolderPath(const QString &userFolderPath) const;
    bool save(FolderEntity &entity, QSqlError *error = nullptr);

private:
    QSqlDatabase database;
};

#endif // FOLDERREPOSITORY_H
