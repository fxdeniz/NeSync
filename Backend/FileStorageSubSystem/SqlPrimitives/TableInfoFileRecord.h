#ifndef TABLEINFOFILERECORD_H
#define TABLEINFOFILERECORD_H

#include <QString>
#include <QSharedPointer>

inline const QString TABLE_NAME_FILE_RECORD = "FileRecord";
inline const QString TABLE_FILE_RECORD_COLNAME_RECORD_ID = "record_id";
inline const QString TABLE_FILE_RECORD_COLNAME_FILE_NAME = "file_name";
inline const QString TABLE_FILE_RECORD_COLNAME_FILE_EXTENSION = "file_extension";
inline const QString TABLE_FILE_RECORD_COLNAME_SYMBOL_DIRECTORY = "symbol_directory";
inline const QString TABLE_FILE_RECORD_COLNAME_SYMBOL_FILE_PATH = "symbol_file_path";
inline const QString TABLE_FILE_RECORD_COLNAME_USER_DIRECTORY = "user_directory";
inline const QString TABLE_FILE_RECORD_COLNAME_USER_FILE_PATH = "user_file_path";
inline const QString TABLE_FILE_RECORD_COLNAME_IS_FAVORITE = "is_favorite";
inline const QString TABLE_FILE_RECORD_COLNAME_IS_FROZEN = "is_frozen";
inline const QString TABLE_FILE_RECORD_COLNAME_IS_AUTO_SYNC_ENABLED = "is_auto_sync_enabled";

class RowFileRecord;

typedef QSharedPointer<RowFileRecord> PtrTo_RowFileRecord;
typedef QSharedPointer<const RowFileRecord> PtrToConst_RowFileRecord;
typedef const QSharedPointer<RowFileRecord> ConstPtrTo_RowFileRecord;
typedef const QSharedPointer<const RowFileRecord> ConstPtrToConst_RowFileRecord;

#endif // TABLEINFOFILERECORD_H
