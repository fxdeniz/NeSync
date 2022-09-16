#ifndef FILEREQUESTRESULT_H
#define FILEREQUESTRESULT_H

#include "Backend/FileStorageSubSystem/SqlPrimitives/RowFileRecord.h"
#include "FileVersionRequestResult.h"

#include <QIcon>

class FileRequestResult
{
public:
    friend class FileStorageManager;
    friend class FolderRequestResult;

    bool isExist() const;

    const QString &fileName() const;
    const QString &fileExtension() const;
    const QString &symbolDirectory() const;
    const QString &symbolFilePath() const;
    const QString &userDirectory() const;
    const QString &userFilePath() const;
    bool isFavorite() const;
    bool isFrozen() const;
    bool isAutoSyncEnabled() const;
    const QIcon &fileIcon() const;
    QList<qlonglong> versionNumbers();
    qlonglong latestVersionNumber() const;

private:
    FileRequestResult();
    FileRequestResult(PtrTo_RowFileRecord row);

    bool exist;

    QString _fileName;
    QString _fileExtension;
    QString _symbolDirectory;
    QString _symbolFilePath;
    QString _userDirectory;
    QString _userFilePath;
    bool _favorite;
    bool _frozen;
    bool _autoSyncEnabled;
    QIcon _icon;

    QList<qlonglong> _versionNumbers;
    qlonglong _latestVersionNumber;
};

#endif // FILEREQUESTRESULT_H
