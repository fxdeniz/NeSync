#ifndef FOLDERENTITY_H
#define FOLDERENTITY_H

#include <QString>
#include <QList>

class FolderEntity
{
public:
    friend class FolderRepository;

    FolderEntity();

    QString suffixPath;
    QString parentFolderPath;
    QString userFolderPath;
    bool isFrozen;

    QString symbolFolderPath() const;
    bool isExist() const;

    QList<FolderEntity> getChildFolders() const;

private:
    QList<FolderEntity> childFolders;

    void setIsExist(bool newIsExist);
    bool _isExist;
};

#endif // FOLDERENTITY_H
