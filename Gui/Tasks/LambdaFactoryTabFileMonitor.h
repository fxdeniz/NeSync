#ifndef LAMBDAFACTORYTABFILEMONITOR_H
#define LAMBDAFACTORYTABFILEMONITOR_H

#include <QString>
#include <QSqlQuery>

class LambdaFactoryTabFileMonitor
{
public:
    static std::function<bool (QString)> lambdaIsFileExistInDb();
    static std::function<bool (QString, QString)> lambdaIsFileRowExistInModelDb();
    static std::function<QSqlQuery (QString, QString)> lambdaFetchFileRowFromModelDb();
    static std::function<void (QString, QString)> lambdaInsertModifiedFileIntoModelDb();
};

#endif // LAMBDAFACTORYTABFILEMONITOR_H
