#ifndef FILEVERSIONREQUESTRESULT_H
#define FILEVERSIONREQUESTRESULT_H

#include "Backend/FileStorageSubSystem/SqlPrimitives/RowFileVersion.h"

class FileVersionRequestResult
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
    FileVersionRequestResult();
    FileVersionRequestResult(PtrTo_RowFileVersion row);

    bool exist;

    QString _symbolFilePathToParent;
    qlonglong _versionNumber;
    qlonglong _size;
    QDateTime _timestamp;
    QString _description;

};

#endif // FILEVERSIONREQUESTRESULT_H
