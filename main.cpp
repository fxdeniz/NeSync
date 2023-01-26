#include <QStandardPaths>
#include <QApplication>
#include <QDebug>
#include "Gui/MainWindow.h"
#include "Backend/FileStorageSubSystem/FileStorageManager.h"
#include "Backend/FileMonitorSubSystem/FileMonitoringManager.h"
#include "Backend/FileMonitorSubSystem/FileSystemEventDb.h"
#include "Utility/DatabaseRegistry.h"
#include "FileStorageSubSystem/ORM/Repository/FolderRepository.h"
#include "FileStorageSubSystem/ORM/Repository/FileRepository.h"
#include "FileStorageSubSystem/ORM/Repository/FileVersionRepository.h"
#include "FileStorageSubSystem/V2_FileStorageManager.h"

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

    V2_FileStorageManager v2fsm(storageDb);
    v2fsm.addNewFolder("/", "test_folder/", "/home/user/Desktop/data");

    return app.exec();
}
