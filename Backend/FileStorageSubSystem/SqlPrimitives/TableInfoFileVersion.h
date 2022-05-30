#ifndef TABLEINFOFILEVERSION_H
#define TABLEINFOFILEVERSION_H

#include <QString>
#include <QSharedPointer>

inline const QString TABLE_NAME_FILE_VERSION = "FileVersion";
inline const QString TABLE_FILE_VERSION_COLNAME_VERSION_ID = "version_id";
inline const QString TABLE_FILE_VERSION_COLNAME_PARENT_RECORD_ID = "parent_record_id";
inline const QString TABLE_FILE_VERSION_COLNAME_VERSION_NUMBER = "version_number";
inline const QString TABLE_FILE_VERSION_COLNAME_INTERNAL_FILE_NAME = "internal_file_name";
inline const QString TABLE_FILE_VERSION_COLNAME_SIZE = "size";
inline const QString TABLE_FILE_VERSION_COLNAME_TIMESTAMP = "timestamp";
inline const QString TABLE_FILE_VERSION_COLNAME_DESCRIPTION = "description";
inline const QString TABLE_FILE_VERSION_COLNAME_HASH = "hash";

class RowFileVersion;

typedef QSharedPointer<RowFileVersion> PtrTo_RowFileVersion;
typedef QSharedPointer<const RowFileVersion> PtrToConst_RowFileVersion;
typedef const QSharedPointer<RowFileVersion> ConstPtrTo_RowFileVersion;
typedef const QSharedPointer<const RowFileVersion> ConstPtrToConst_RowFileVersion;

#endif // TABLEINFOFILEVERSION_H
