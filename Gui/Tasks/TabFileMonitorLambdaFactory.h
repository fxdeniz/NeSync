#ifndef TABFILEMONITORLAMBDAFACTORY_H
#define TABFILEMONITORLAMBDAFACTORY_H

#include <QString>
#include <QSqlQuery>

class TabFileMonitorLambdaFactory
{
public:
    static std::function<bool (QString)> lambdaIsFileExistInDb();
    static std::function<QSqlQuery (QString, QString)> lambdaFetchFileRowFromModelDb();
    static std::function<void (QString, QString)> lambdaInsertModifiedFileIntoModelDb();
};

#endif // TABFILEMONITORLAMBDAFACTORY_H
