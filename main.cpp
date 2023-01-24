#include <QStandardPaths>
#include <QApplication>
#include <QDebug>
#include "Gui/MainWindow.h"
#include "Backend/FileStorageSubSystem/FileStorageManager.h"
#include "Backend/FileMonitorSubSystem/FileMonitoringManager.h"
#include "Backend/FileMonitorSubSystem/FileSystemEventDb.h"
#include "Utility/DatabaseRegistry.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    //FileStorageManager storageManager(app.applicationDirPath() + "/backup", "C:/Users/AppData/Local");

//    auto testFolderPath = QStandardPaths::writableLocation(QStandardPaths::StandardLocation::DesktopLocation);
//    testFolderPath += "/test/";

//    QStringList predictionList;
//    predictionList << testFolderPath;
//    predictionList << "not_exist";

//    FileMonitoringManagerIntegrationTest fmmITest(predictionList);

    MainWindow w;
    w.show();

    QSqlDatabase storageDb = DatabaseRegistry::fileStorageDatabase();
    storageDb.open();

    qDebug() << "isStorageDbOpen = " << storageDb.isOpen();

//    FileSystemEventDb database;
//    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::StandardLocation::DesktopLocation);
//    QString filePath = desktopPath + QDir::separator() + "data" + QDir::separator() + "test.txt";
//    database.addFolder(desktopPath);
//    database.addFile(filePath);
//    database.deleteFolder(desktopPath);
//    database.deleteFile(filePath);

    return app.exec();
}
