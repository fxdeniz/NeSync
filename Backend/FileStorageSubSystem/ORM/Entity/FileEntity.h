#ifndef FILEENTITY_H
#define FILEENTITY_H

#include <QString>

class FileEntity
{
public:
    friend class FileRepository;
    friend class FolderRepository;

    FileEntity();

    QString fileName;
    QString symbolFolderPath;
    bool isFrozen;

    QString symbolFilePath() const;

    bool isExist() const;

private:
    void setIsExist(bool newIsExist);
    bool _isExist;
};

#endif // FILEENTITY_H
