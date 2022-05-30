#include "MonitoredDirDb.h"

#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <QDir>

MonitoredDirDb::MonitoredDirDb()
{
    // this->db = QSqlDatabase::addDatabase("QSQLITE", "file_monitor_in_mem_db");
    // db.setDatabaseName(":memory:");

    this->db = QSqlDatabase::addDatabase("QSQLITE", "file_monitor_in_mem_db");
    db.setDatabaseName("C:\\Users\\MiniDeniz\\Desktop\\zmem.db3");

    db.open();

    this->db.exec("PRAGMA foreign_keys = ON;");
    this->createDatabase();
}

bool MonitoredDirDb::isDirExistByPath(const QString &dir) const
{
    bool result = false;

    auto _dir = this->standardizeDir(dir);

    QString queryTemplate = "SELECT * FROM Dir WHERE dir_path = :1;" ;

    QSqlQuery query(this->db);
    query.prepare(queryTemplate);
    query.bindValue(":1", _dir);
    query.exec();

    if(query.next())
        result = true;

    return result;
}

bool MonitoredDirDb::isDirExistByEfswWatchID(efsw::WatchID id) const
{
    bool result = false;
    QString queryTemplate = "SELECT * FROM Dir WHERE efsw_watch_id = :1;" ;

    QSqlQuery query(this->db);
    query.prepare(queryTemplate);
    query.bindValue(":1", (qlonglong) id);
    query.exec();

    if(query.next())
        result = true;

    return result;
}

bool MonitoredDirDb::isFileExistInDir(const QString &fileName, const QString &dir) const
{
    bool result = false;
    auto _dir = this->standardizeDir(dir);
    auto pathToFile = _dir + fileName;

    QString queryTemplate = "SELECT * FROM File WHERE file_path = :1;" ;

    QSqlQuery query(this->db);
    query.prepare(queryTemplate);
    query.bindValue(":1", pathToFile);
    query.exec();

    if(query.next())
        result = true;

    return result;
}

bool MonitoredDirDb::addDir(const QString &dir, efsw::WatchID id)
{
    bool result = false;

    auto _dir = this->standardizeDir(dir);

    QString queryTemplate = "INSERT INTO Dir (dir_path, efsw_watch_id)";
    queryTemplate += " VALUES (:1, :2);" ;

    QSqlQuery query(this->db);
    query.prepare(queryTemplate);
    query.bindValue(":1", _dir);
    query.bindValue(":2", (qlonglong)id);
    query.exec();

    if(query.numRowsAffected() >= 1
        && query.lastError().type() == QSqlError::ErrorType::NoError)
    {
        result = true;
    }

    return result;
}

bool MonitoredDirDb::removeDir(const QString &dir)
{
    bool result = false;
    auto _dir = this->standardizeDir(dir);

    QString queryTemplate = "DELETE FROM Dir WHERE dir_path = :1;" ;

    QSqlQuery query(this->db);
    query.prepare(queryTemplate);
    query.bindValue(":1", _dir);
    query.exec();

    if(query.numRowsAffected() >= 1
       && query.lastError().type() == QSqlError::ErrorType::NoError)
    {
        result = true;
    }

    return result;
}

bool MonitoredDirDb::renameDir(const QString &oldDir, const QString &newDir)
{
    bool result = false;
    auto _oldDir = this->standardizeDir(oldDir);
    auto _newDir = this->standardizeDir(newDir);

    QString queryTemplate = "UPDATE Dir SET dir_path = :1 WHERE dir_path = :2;" ;

    QSqlQuery query(this->db);
    query.prepare(queryTemplate);
    query.bindValue(":1", _newDir);
    query.bindValue(":2", _oldDir);
    query.exec();

    if(query.numRowsAffected() >= 1
        && query.lastError().type() == QSqlError::ErrorType::NoError)
    {
        result = true;
    }

    return result;
}

bool MonitoredDirDb::addFileToDir(const QString &fileName, const QString &dir)
{
    bool result = false;
    auto _dir = this->standardizeDir(dir);

    QFileInfo info(_dir + fileName);
    QString queryTemplate = "INSERT INTO File (dir_path, file_name, file_extension) VALUES (:1, :2, :3);" ;

    QSqlQuery query(this->db);
    query.prepare(queryTemplate);
    query.bindValue(":1", _dir);
    query.bindValue(":2", info.baseName());

    if(info.completeSuffix().isNull() || info.completeSuffix().isEmpty())
        query.bindValue(":3", "");
    else
        query.bindValue(":3", "." + info.completeSuffix());

    query.exec();

    if(query.numRowsAffected() >= 1
        && query.lastError().type() == QSqlError::ErrorType::NoError)
    {
        result = true;
    }

    return result;
}

bool MonitoredDirDb::removeFileFromDir(const QString &fileName, const QString &dir)
{
    bool result = false;
    auto _dir = this->standardizeDir(dir);

    auto filePath = _dir + fileName;
    QString queryTemplate = "DELETE FROM File WHERE file_path = :1;" ;

    QSqlQuery query(this->db);
    query.prepare(queryTemplate);
    query.bindValue(":1", filePath);
    query.exec();

    if(query.numRowsAffected() >= 1
        && query.lastError().type() == QSqlError::ErrorType::NoError)
    {
        result = true;
    }

    return result;
}

bool MonitoredDirDb::renameFileInDir(const QString &oldFileName, const QString &newFileName, const QString &dir)
{
    bool result = false;
    auto _dir = this->standardizeDir(dir);

    QFileInfo info(_dir + newFileName);

    QString queryTemplate = "UPDATE File";
    queryTemplate += " SET file_name = :1, file_extension = :2";
    queryTemplate += " WHERE file_path = :3;" ;

    QSqlQuery query(this->db);
    query.prepare(queryTemplate);
    query.bindValue(":1", info.baseName());
    if(info.completeSuffix().isNull() || info.completeSuffix().isEmpty())
        query.bindValue(":2", "");
    else
        query.bindValue(":2", "." + info.completeSuffix());

    query.bindValue(":3", _dir + oldFileName);
    query.exec();

    if(query.numRowsAffected() >= 1
        && query.lastError().type() == QSqlError::ErrorType::NoError)
    {
        result = true;
    }

    return result;
}

bool MonitoredDirDb::updateOldFileNameOfFileInDir(const QString &fileName, const QString &oldFileName, const QString &dir)
{
    bool result = false;
    auto _dir = this->standardizeDir(dir);

    QString queryTemplate = "UPDATE File SET old_file_name = :1 WHERE file_path = :2;" ;

    QSqlQuery query(this->db);
    query.prepare(queryTemplate);
    query.bindValue(":1", oldFileName);
    query.bindValue(":2", _dir + fileName);
    query.exec();

    if(query.numRowsAffected() >= 1
        && query.lastError().type() == QSqlError::ErrorType::NoError)
    {
        result = true;
    }

    return result;
}

QString MonitoredDirDb::oldFileNameOfFileInDir(const QString &fileName, const QString &dir) const
{
    QString result = "";

    auto _dir = this->standardizeDir(dir);

    QString queryTemplate = "SELECT * FROM File WHERE file_path = :1;" ;

    QSqlQuery query(this->db);
    query.prepare(queryTemplate);
    query.bindValue(":1", _dir + fileName);
    query.exec();

    if(query.next())
        result = query.record().value("old_file_name").toString();

    return result;
}

bool MonitoredDirDb::updateOldNameOfDir(const QString &oldDirName, const QString &dir)
{
    bool result = false;
    auto _dir = this->standardizeDir(dir);

    QString queryTemplate = "UPDATE Dir SET old_dir_name = :1 WHERE dir_path = :2;" ;

    QSqlQuery query(this->db);
    query.prepare(queryTemplate);
    query.bindValue(":1", oldDirName);
    query.bindValue(":2", _dir);
    query.exec();

    if(query.numRowsAffected() >= 1
        && query.lastError().type() == QSqlError::ErrorType::NoError)
    {
        result = true;
    }

    return result;
}

QString MonitoredDirDb::oldNameOfDir(const QString &dir) const
{
    QString result = "";

    auto _dir = this->standardizeDir(dir);

    QString queryTemplate = "SELECT * FROM Dir WHERE dir_path = :1;" ;

    QSqlQuery query(this->db);
    query.prepare(queryTemplate);
    query.bindValue(":1", _dir);
    query.exec();

    if(query.next())
        result = query.record().value("old_dir_name").toString();

    return result;
}

bool MonitoredDirDb::moveFileBetweenDirs(const QString &fileName, const QString &fromDir, const QString &toDir)
{
    bool result = false;
    auto _fromDir = this->standardizeDir(fromDir);
    auto _toDir = this->standardizeDir(toDir);

    QString queryTemplate = "UPDATE File SET dir_path = :1 WHERE file_path = :2;" ;

    QSqlQuery query(this->db);
    query.prepare(queryTemplate);
    query.bindValue(":1", _toDir);
    query.bindValue(":2", _fromDir + fileName);
    query.exec();

    if(query.numRowsAffected() >= 1
        && query.lastError().type() == QSqlError::ErrorType::NoError)
    {
        result = true;
    }

    return result;
}

bool MonitoredDirDb::updateEfswWatchIDofDir(const QString &dir, efsw::WatchID newID)
{
    bool result = false;
    auto _dir = this->standardizeDir(dir);

    QString queryTemplate = "UPDATE Dir SET efsw_watch_id = :1 WHERE dir_path = :2;" ;

    QSqlQuery query(this->db);
    query.prepare(queryTemplate);
    query.bindValue(":1", (qlonglong)newID);
    query.bindValue(":2", _dir);
    query.exec();

    if(query.numRowsAffected() >= 1
        && query.lastError().type() == QSqlError::ErrorType::NoError)
    {
        result = true;
    }

    return result;
}

qlonglong MonitoredDirDb::efswWatchIDofDir(const QString &dir) const
{
    qlonglong result = -1;

    auto _dir = this->standardizeDir(dir);

    QString queryTemplate = "SELECT * FROM Dir WHERE dir_path = :1;" ;

    QSqlQuery query(this->db);
    query.prepare(queryTemplate);
    query.bindValue(":1", _dir);
    query.exec();

    if(query.next())
        result = query.record().value("efsw_watch_id").toLongLong();

    return result;
}

QList<qlonglong> MonitoredDirDb::efswWatchIDList() const
{
    QList<qlonglong> result;

    QString queryTemplate = "SELECT * FROM Dir;" ;

    QSqlQuery query(this->db);
    query.prepare(queryTemplate);
    query.exec();

    if(query.next())
        result.append(query.record().value("efsw_watch_id").toLongLong());

    return result;
}

int MonitoredDirDb::fileCountInDir(const QString &dir) const
{
    int result = -1;
    auto _dir = this->standardizeDir(dir);

    QString columnName = "result_column";
    QString queryTemplate = "SELECT COUNT(*) AS %1 FROM File WHERE dir_path = :1;" ;
    queryTemplate = queryTemplate.arg(columnName);

    QSqlQuery query(this->db);
    query.prepare(queryTemplate);
    query.bindValue(":1", _dir);
    query.exec();

    if(query.next())
        result = query.record().value(columnName).toInt();

    return result;
}

QStringList MonitoredDirDb::fileListInDir(const QString &dir) const
{
    QStringList result;
    auto _dir = this->standardizeDir(dir);

    QString queryTemplate = "SELECT * FROM File WHERE dir_path = :1;" ;
    QSqlQuery query(this->db);
    query.prepare(queryTemplate);
    query.bindValue(":1", _dir);

    query.exec();

    while(query.next())
    {
        auto record = query.record();
        auto item = record.value("file_name").toString();
        item = item.append(record.value("file_extension").toString());

        result.append(item);
    }

    return result;
}

bool MonitoredDirDb::updateEventTimestampOfDir(const QString &dir, const QDateTime &newTimestamp)
{
    bool result = false;
    auto _dir = this->standardizeDir(dir);

    QString queryTemplate = "UPDATE Dir SET event_timestamp = :1 WHERE dir_path = :2;" ;

    QSqlQuery query(this->db);
    query.prepare(queryTemplate);
    query.bindValue(":1", newTimestamp);
    query.bindValue(":2", _dir);
    query.exec();

    if(query.numRowsAffected() >= 1
        && query.lastError().type() == QSqlError::ErrorType::NoError)
    {
        result = true;
    }

    return result;
}

QDateTime MonitoredDirDb::eventTimestampOfDir(const QString &dir) const
{
    QDateTime result;

    auto _dir = this->standardizeDir(dir);

    QString queryTemplate = "SELECT * FROM Dir WHERE dir_path = :1;" ;

    QSqlQuery query(this->db);
    query.prepare(queryTemplate);
    query.bindValue(":1", _dir);
    query.exec();

    if(query.next())
        result = query.record().value("event_timestamp").toDateTime();

    return result;
}

bool MonitoredDirDb::updateEventTimestampOfFileInDir(const QString &fileName, const QString &dir, const QDateTime &newTimestamp)
{
    bool result = false;
    auto _dir = this->standardizeDir(dir);

    QString queryTemplate = "UPDATE File SET event_timestamp = :1 WHERE file_path = :2;" ;

    QSqlQuery query(this->db);
    query.prepare(queryTemplate);
    query.bindValue(":1", newTimestamp);
    query.bindValue(":2", _dir + fileName);
    query.exec();

    if(query.numRowsAffected() >= 1
        && query.lastError().type() == QSqlError::ErrorType::NoError)
    {
        result = true;
    }

    return result;
}

QDateTime MonitoredDirDb::eventTimestampOfFileInDir(const QString &fileName, const QString &dir) const
{
    QDateTime result;

    auto _dir = this->standardizeDir(dir);

    QString queryTemplate = "SELECT * FROM File WHERE file_path = :1;" ;

    QSqlQuery query(this->db);
    query.prepare(queryTemplate);
    query.bindValue(":1", _dir + fileName);
    query.exec();

    if(query.next())
        result = query.record().value("event_timestamp").toDateTime();

    return result;
}

bool MonitoredDirDb::scheduleDirAs(const QString &dir, MonitoredItemState state)
{
    bool result = false;

    auto _dir = this->standardizeDir(dir);

    QString queryTemplate = "UPDATE Dir SET is_scheduled = :1, state = :2 WHERE dir_path = :3;" ;

    QSqlQuery query(this->db);
    query.prepare(queryTemplate);
    query.bindValue(":1", true);
    query.bindValue(":2", state);
    query.bindValue(":3", _dir);
    query.exec();

    if(query.numRowsAffected() >= 1
        && query.lastError().type() == QSqlError::ErrorType::NoError)
    {
        result = true;
    }

    return result;
}

QStringList MonitoredDirDb::scheduledDirList(MonitoredItemState criteria) const
{
    QStringList result;

    QString queryTemplate = "SELECT dir_path FROM Dir WHERE is_scheduled = 1 AND state = :1;" ;
    QSqlQuery query(this->db);
    query.prepare(queryTemplate);
    query.bindValue(":1", criteria);

    query.exec();

    while(query.next())
    {
        auto item = query.record().value("dir_path").toString();
        result.append(item);
    }

    return result;
}

bool MonitoredDirDb::unScheduleDir(const QString &dir)
{
    bool result = false;

    auto _dir = this->standardizeDir(dir);

    QString queryTemplate = "UPDATE Dir SET is_scheduled = :1, state = :2, event_timestamp = :3 WHERE dir_path = :4;" ;

    QSqlQuery query(this->db);
    query.prepare(queryTemplate);
    query.bindValue(":1", false);
    query.bindValue(":2", MonitoredItemState::Monitored);
    query.bindValue(":3", "");
    query.bindValue(":4", _dir);
    query.exec();

    if(query.numRowsAffected() >= 1
        && query.lastError().type() == QSqlError::ErrorType::NoError)
    {
        result = true;
    }

    return result;
}

QStringList MonitoredDirDb::scheduledFileList(MonitoredItemState criteria) const
{
    QStringList result;

    QString queryTemplate = "SELECT file_path FROM File WHERE is_scheduled = 1 AND state = :1;" ;
    QSqlQuery query(this->db);
    query.prepare(queryTemplate);
    query.bindValue(":1", criteria);

    query.exec();

    while(query.next())
    {
        auto item = query.record().value("file_path").toString();
        result.append(item);
    }

    return result;
}

bool MonitoredDirDb::scheduleFileInDirAs(const QString &fileName, const QString &dir, MonitoredItemState state)
{
    bool result = false;

    auto _dir = this->standardizeDir(dir);

    QString queryTemplate = "UPDATE File SET is_scheduled = :1, state = :2 WHERE file_path = :3;" ;

    QSqlQuery query(this->db);
    query.prepare(queryTemplate);
    query.bindValue(":1", true);
    query.bindValue(":2", state);
    query.bindValue(":3", _dir + fileName);
    query.exec();

    if(query.numRowsAffected() >= 1
        && query.lastError().type() == QSqlError::ErrorType::NoError)
    {
        result = true;
    }

    return result;
}

bool MonitoredDirDb::unScheduleFileInDir(const QString &fileName, const QString &dir)
{
    bool result = false;

    auto _dir = this->standardizeDir(dir);

    QString queryTemplate = "UPDATE File SET is_scheduled = :1, state = :2, event_timestamp = :3 WHERE file_path = :4;" ;

    QSqlQuery query(this->db);
    query.prepare(queryTemplate);
    query.bindValue(":1", false);
    query.bindValue(":2", MonitoredItemState::Monitored);
    query.bindValue(":3", "");
    query.bindValue(":4", _dir + fileName);
    query.exec();

    if(query.numRowsAffected() >= 1
        && query.lastError().type() == QSqlError::ErrorType::NoError)
    {
        result = true;
    }

    return result;
}

MonitoredDirDb::MonitoredItemState MonitoredDirDb::stateOfDir(const QString &dir) const
{
    MonitoredItemState result = MonitoredItemState::InValid;

    auto _dir = this->standardizeDir(dir);

    QString queryTemplate = "SELECT * FROM Dir WHERE dir_path = :1;" ;

    QSqlQuery query(this->db);
    query.prepare(queryTemplate);
    query.bindValue(":1", _dir);
    query.exec();

    if(query.next())
        result = query.record().value("state").value<MonitoredItemState>();

    return result;
}

MonitoredDirDb::MonitoredItemState MonitoredDirDb::stateOfFileInDir(const QString &fileName, const QString &dir) const
{
    MonitoredItemState result = MonitoredItemState::InValid;

    auto _dir = this->standardizeDir(dir);

    QString queryTemplate = "SELECT * FROM File WHERE file_path = :1;" ;

    QSqlQuery query(this->db);
    query.prepare(queryTemplate);
    query.bindValue(":1", _dir + fileName);
    query.exec();

    if(query.next())
        result = query.record().value("state").value<MonitoredItemState>();

    return result;
}

bool MonitoredDirDb::hasContainScheduledFiles() const
{
    bool result = false;

    QString queryTemplate = "SELECT * FROM File WHERE is_scheduled = :1;" ;

    QSqlQuery query(this->db);
    query.prepare(queryTemplate);
    query.bindValue(":1", true);
    query.exec();

    if(query.next())
        result = true;

    return result;
}

bool MonitoredDirDb::resetDb()
{
    bool result = false;

    bool isAllTablesDropped = this->dropAllTables();

    if(isAllTablesDropped)
    {
        this->createDatabase();
        result = true;
    }

    return result;
}

void MonitoredDirDb::createDatabase()
{
    QString queryCreateTableDir;
    queryCreateTableDir += " CREATE TABLE Dir ( ";
    queryCreateTableDir += " dir_path TEXT NOT NULL UNIQUE, ";
    queryCreateTableDir += " efsw_watch_id INTEGER NOT NULL UNIQUE, ";
    queryCreateTableDir += " state	INTEGER NOT NULL DEFAULT 0, ";
    queryCreateTableDir += " event_timestamp TEXT, ";
    queryCreateTableDir += " old_dir_name TEXT UNIQUE, ";
    queryCreateTableDir += " is_scheduled INTEGER NOT NULL DEFAULT 0 CHECK(is_scheduled == 0 OR is_scheduled == 1), ";
    queryCreateTableDir += " PRIMARY KEY(dir_path) ";
    queryCreateTableDir += " ); ";

    QString queryCreateTableFile;
    queryCreateTableFile += " CREATE TABLE File ( ";
    queryCreateTableFile += " dir_path	TEXT NOT NULL, ";
    queryCreateTableFile += " file_name	TEXT NOT NULL, ";
    queryCreateTableFile += " file_extension TEXT, ";
    queryCreateTableFile += " file_path	TEXT NOT NULL UNIQUE GENERATED ALWAYS AS (dir_path || file_name || file_extension) VIRTUAL, ";
    queryCreateTableFile += " state	INTEGER NOT NULL DEFAULT 0, ";
    queryCreateTableFile += " old_file_name TEXT, ";
    queryCreateTableFile += " event_timestamp TEXT, ";
    queryCreateTableFile += " is_scheduled INTEGER NOT NULL DEFAULT 0 CHECK(is_scheduled == 0 OR is_scheduled == 1), ";
    queryCreateTableFile += " PRIMARY KEY(dir_path, file_name, file_extension), ";
    queryCreateTableFile += " FOREIGN KEY(dir_path) REFERENCES Dir(dir_path) ON DELETE CASCADE ON UPDATE CASCADE ";
    queryCreateTableFile += " ); ";


    this->db.exec(queryCreateTableDir);
    this->db.exec(queryCreateTableFile);
}

bool MonitoredDirDb::dropAllTables()
{
    bool result = false;

    QString queryTemplateDropFileTable = "DROP TABLE File;" ;
    QString queryTemplateDropDirTable = "DROP TABLE Dir;" ;

    QSqlQuery queryDropFileTable(this->db);
    queryDropFileTable.prepare(queryTemplateDropFileTable);

    QSqlQuery queryDropDirTable(this->db);
    queryDropDirTable.prepare(queryTemplateDropDirTable);

    if(this->db.transaction())
    {
        queryDropFileTable.exec();
        queryDropDirTable.exec();

        if(queryDropFileTable.lastError().type() != QSqlError::ErrorType::NoError
            || queryDropDirTable.lastError().type() != QSqlError::ErrorType::NoError)
        {
            this->db.rollback();
            qDebug() << "cann't roolback because = " << this->db.lastError();
        }
        else
        {
            this->db.commit();
            result = true;
        }
    }

    return result;
}

QString MonitoredDirDb::standardizeDir(const QString &dir) const
{
    auto result = QDir::toNativeSeparators(dir);
    auto seprator = QDir::separator();

    bool isEndsWithSeprator = result.endsWith(seprator);
    if(!isEndsWithSeprator)
        result.append(seprator);

    return result;
}
