#ifndef BASESQLPRIMITIVE_H
#define BASESQLPRIMITIVE_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QDateTime>

inline const qlonglong INVALID_FIELD_VALUE_QLONGLONG = -1;
inline const int INVALID_FIELD_VALUE_INT = -1;
inline const QString INVALID_FIELD_VALUE_QSTRING = "";
inline const bool INVALID_FIELD_VALUE_BOOL = false;
inline const QDateTime INVALID_FIELD_VALUE_QDATETIME = QDateTime();

class BaseSqlPrimitive
{
public:
    BaseSqlPrimitive(const QSqlDatabase db, const QString &tableName);

    const QString &getTableName() const;
    const QSqlError &getLastError() const;

    virtual ~BaseSqlPrimitive();

protected:
    const QSqlDatabase &getDb() const;

    void setDb(const QSqlDatabase &newDb);
    void setLastError(const QSqlError &newLastError);
    void setTableName(const QString &newTableName);

private:
    QSqlDatabase db;
    QString tableName;
    QSqlError lastError;
};

#endif // BASESQLPRIMITIVE_H
