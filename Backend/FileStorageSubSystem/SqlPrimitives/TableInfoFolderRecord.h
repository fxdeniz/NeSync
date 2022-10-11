#ifndef TABLEINFOFOLDERRECORD_H
#define TABLEINFOFOLDERRECORD_H

#include <QString>
#include <QSharedPointer>

inline const QString TABLE_NAME_FOLDER_RECORD = "FolderRecord";
inline const QString TABLE_FOLDER_RECORD_COLNAME_FOLDER_ID = "folder_id";
inline const QString TABLE_FOLDER_RECORD_COLNAME_PARENT_DIRECTORY = "parent_directory";
inline const QString TABLE_FOLDER_RECORD_COLNAME_SUFFIX_DIRECTORY = "suffix_directory";
inline const QString TABLE_FOLDER_RECORD_COLNAME_USER_DIRECTORY = "user_directory";
inline const QString TABLE_FOLDER_RECORD_COLNAME_DIRECTORY = "directory";
inline const QString TABLE_FOLDER_RECORD_COLNAME_IS_FAVORITE = "is_favorite";

class RowFolderRecord;

typedef QSharedPointer<RowFolderRecord> PtrTo_RowFolderRecord;
typedef QSharedPointer<const RowFolderRecord> PtrToConst_RowFolderRecord;
typedef const QSharedPointer<RowFolderRecord> ConstPtrTo_RowFolderRecord;
typedef const QSharedPointer<const RowFolderRecord> ConstPtrToConst_RowFolderRecord;

#endif // TABLEINFOFOLDERRECORD_H
