#ifndef LAMBDAFACTORYTABFILEMONITOR_H
#define LAMBDAFACTORYTABFILEMONITOR_H

#include "DataModels/TabFileMonitor/V2TableModelFileMonitor.h"

#include <QString>
#include <QSqlQuery>

class LambdaFactoryTabFileMonitor
{
public:
    static std::function<bool (QString)> lambdaIsFileExistInDb();
    static std::function<bool (QString, QString)> lambdaIsFileRowExistInModelDb();
    static std::function<QSqlQuery (QString, QString)> lambdaFetchFileRowFromModelDb();
    static std::function<void (QString, QString, V2TableModelFileMonitor::TableItemStatus)> lambdaInsertFileRowIntoModelDb();
    static std::function<void (QString, QString, V2TableModelFileMonitor::TableItemStatus)> lambdaUpdateStatusOfFileRowInModelDb();
};

#endif // LAMBDAFACTORYTABFILEMONITOR_H
