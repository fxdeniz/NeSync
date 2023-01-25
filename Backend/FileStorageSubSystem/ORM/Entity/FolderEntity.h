#ifndef FOLDERENTITY_H
#define FOLDERENTITY_H

#include <QString>

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

private:
    void setIsExist(bool newIsExist);

    bool _isExist;
};

#endif // FOLDERENTITY_H
