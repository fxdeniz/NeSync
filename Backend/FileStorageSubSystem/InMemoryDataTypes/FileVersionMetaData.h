#ifndef FILEVERSIONMETADATA_H
#define FILEVERSIONMETADATA_H

#include "Backend/FileStorageSubSystem/SqlPrimitives/RowFileVersion.h"

class FileVersionMetaData
{
public:
    friend class FileStorageManager;

    bool isExist() const;

    qlonglong versionNumber() const;
    qlonglong fileSize() const;
    const QDateTime &timestamp() const;
    const QString &description() const;

    const QString &symbolFilePathToParent() const;

private:
    FileVersionMetaData();
    FileVersionMetaData(PtrTo_RowFileVersion row);

    bool exist;

    QString _symbolFilePathToParent;
    qlonglong _versionNumber;
    qlonglong _size;
    QDateTime _timestamp;
    QString _description;

};

#endif // FILEVERSIONMETADATA_H
