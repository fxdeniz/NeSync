#include <QApplication>
#include <QDebug>
#include "Gui/MainWindow.h"
#include "Backend/FileStorageSubSystem/FileStorageManager.h"
#include "Backend/FileMonitorSubSystem/FileMonitoringManager.h"
#include "Backend/FileMonitorSubSystem/FileMonitoringManagerIntegrationTest.h"
#include "Backend/FileMonitorSubSystem/MonitoredDirDb.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    FileStorageManager storageManager(app.applicationDirPath() + "/backup", "C:/Users/AppData/Local");

    QStringList predictionList;
    predictionList << "C:/Users/MiniDeniz/Desktop/test/";
    predictionList << "not_exist";

    FileMonitoringManagerIntegrationTest fmmITest(predictionList);

    MainWindow w;
    w.show();

    return app.exec();
}
