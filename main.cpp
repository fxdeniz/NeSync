#include <QCoreApplication>
#include <QDebug>
#include "Backend/FileStorageSubSystem/FileStorageManager.h"
#include "Backend/FileMonitorSubSystem/FileMonitoringManager.h"
#include "Backend/FileMonitorSubSystem/FileMonitoringManagerIntegrationTest.h"
#include "Backend/FileMonitorSubSystem/MonitoredDirDb.h"
#include <QFile>


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    FileStorageManager storageManager(app.applicationDirPath() + "/backup", "C:/Users/AppData/Local");

    QStringList predictionList;
    predictionList << "C:/Users/MiniDeniz/Desktop/test/";
    predictionList << "not_exist";

    FileMonitoringManagerIntegrationTest fmmITest(predictionList);

    return app.exec();
}
