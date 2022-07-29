#ifndef LAMBDAFACTORYTABFILEMONITOR_H
#define LAMBDAFACTORYTABFILEMONITOR_H

#include "DataModels/TabFileMonitor/V2TableModelFileMonitor.h"

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
    static std::function<V2TableModelFileMonitor::TableItemStatus (QString, QString)> lambdaFetchStatusOfRowFromModelDb();
    static std::function<void (QString, QString, V2TableModelFileMonitor::TableItemStatus)> lambdaInsertRowIntoModelDb();
    static std::function<void (QString, QString, V2TableModelFileMonitor::TableItemStatus)> lambdaUpdateStatusOfRowInModelDb();
    static std::function<void (QString, QString)> lambdaDeleteRowFromModelDb();
    static std::function<void (QString, QString, QString)> lambdaUpdateOldNameOfRowInModelDb();
    static std::function<void (QString, QString, QString)> lambdaUpdateNameOfRowInModelDb();
};

#endif // LAMBDAFACTORYTABFILEMONITOR_H
