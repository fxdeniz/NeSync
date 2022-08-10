#ifndef LAMBDAFACTORYTABFILEMONITOR_H
#define LAMBDAFACTORYTABFILEMONITOR_H

#include "DataModels/TabFileMonitor/TableModelFileMonitor.h"

#include <QString>
#include <QSqlQuery>

class LambdaFactoryTabFileMonitor
{
public:
    static std::function<bool (QString)> lambdaIsFileExistInDb();
    static std::function<bool (QString)> lambdaIsFolderExistInDb();
    static std::function<bool (QString, QString)> lambdaIsRowExistInModelDb();
    static std::function<QSqlQuery (QString, QString)> lambdaFetchFileRowFromModelDb();
    static std::function<bool (QString, QString)> lambdaIsFileRowReanmedInModelDb();
    static std::function<TableModelFileMonitor::ItemStatus (QString, QString)> lambdaFetchStatusOfRowFromModelDb();
    static std::function<void (QString, QString, TableModelFileMonitor::ItemStatus)> lambdaInsertRowIntoModelDb();
    static std::function<void (QString, QString, TableModelFileMonitor::ItemStatus)> lambdaUpdateStatusOfRowInModelDb();
    static std::function<void (QString, QString)> lambdaDeleteRowFromModelDb();
    static std::function<void (QString, QString, QString)> lambdaUpdateOldNameOfRowInModelDb();
    static std::function<void (QString, QString, QString)> lambdaUpdateNameOfRowInModelDb();
};

#endif // LAMBDAFACTORYTABFILEMONITOR_H
