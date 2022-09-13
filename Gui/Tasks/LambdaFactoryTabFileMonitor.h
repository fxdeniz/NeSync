#ifndef LAMBDAFACTORYTABFILEMONITOR_H
#define LAMBDAFACTORYTABFILEMONITOR_H

#include "DataModels/TabFileMonitor/TableModelFileMonitor.h"

#include <QString>
#include <QSqlQuery>

class LambdaFactoryTabFileMonitor
{
public:
    static std::function<bool (QString)> isFileExistInDb();
    static std::function<bool (QString)> isFolderExistInDb();
    static std::function<bool (QString, QString)> isRowExistInModelDb();
    static std::function<QSqlQuery (QString, QString)> fetchFileRowFromModelDb();
    static std::function<bool (QString, QString)> isFileRowReanmedInModelDb();
    static std::function<TableModelFileMonitor::ItemStatus (QString, QString)> fetchStatusOfRowFromModelDb();
    static std::function<QString (QString, QString)> fetchNameOfRowFromModelDb();
    static std::function<QString (QString, QString)> fetchOldNameOfRowFromModelDb();
    static std::function<void (QString, QString, TableModelFileMonitor::ItemStatus)> insertRowIntoModelDb();
    static std::function<void (QString, QString, TableModelFileMonitor::ItemStatus)> updateStatusOfRowInModelDb();
    static std::function<void (QString, QString, TableModelFileMonitor::ProgressStatus)> updateProgressOfRowInModelDb();
    static std::function<void (QString, QString)> deleteRowFromModelDb();
    static std::function<void (QString, QString, QString)> updateOldNameOfRowInModelDb();
    static std::function<void (QString, QString, QString)> updateNameOfRowInModelDb();
    static std::function<QStringList (QString,
                                      TableModelFileMonitor::ItemType,
                                      TableModelFileMonitor::ProgressStatus)> fetchRowsByProgressFromModelDb();
    static std::function<bool (QString, QString)> applyActionForFolder();
    static std::function<bool (QString, QString)> applyActionForFile();

private:
    static QString generateSymbolFolderPathFromUserDir(const QString userFolderPath);
};

#endif // LAMBDAFACTORYTABFILEMONITOR_H
