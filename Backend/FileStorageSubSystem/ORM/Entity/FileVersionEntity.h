#ifndef FILEVERSIONENTITY_H
#define FILEVERSIONENTITY_H

#include <QString>
#include <QDateTime>

class FileVersionEntity
{
public:
    friend class FileVersionRepository;
    friend class FileRepository;

    FileVersionEntity();

    QString symbolFilePath;
    qlonglong versionNumber;
    QString internalFileName;
    qlonglong size;
    QDateTime timestamp;
    QString description;
    QString hash;

    bool isExist() const;

    QPair<QString, qlonglong> getPrimaryKey() const;

private:
    void setPrimaryKey(QString &symbolFilePath, qlonglong versionNumber);
    QPair<QString, qlonglong> primaryKey;

    void setIsExist(bool newIsExist);
    bool _isExist;
};

#endif // FILEVERSIONENTITY_H
