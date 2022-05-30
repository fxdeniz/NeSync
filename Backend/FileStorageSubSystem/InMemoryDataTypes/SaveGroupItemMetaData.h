#ifndef SAVEGROUPITEMMETADATA_H
#define SAVEGROUPITEMMETADATA_H

#include "Backend/FileStorageSubSystem/SqlPrimitives/RowSaveGroupItem.h"

class SaveGroupItemMetaData
{
public:
    friend class FileStorageManager;

    SaveGroupItemMetaData();

    bool isExist() const;

    const QString &symbolPathToParentFile() const;
    qlonglong parentVersionNumber() const;
    qlonglong saveGroupNumber() const;
    const QDateTime &originalTimestamp() const;
    const QString &originalDescription() const;

    QString toString() const;

private:
    SaveGroupItemMetaData(PtrTo_RowSaveGroupItem row);

    bool exist;

    QString _symbolPathToParentFile;
    qlonglong _parentVersionNumber;
    qlonglong _saveGroupNumber;
    QDateTime _originalTimestamp;
    QString _originalDescription;
};

#endif // SAVEGROUPITEMMETADATA_H
