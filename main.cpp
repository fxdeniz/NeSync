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

    QString tempPath = QStandardPaths::writableLocation(QStandardPaths::StandardLocation::TempLocation);
    tempPath = QDir::toNativeSeparators(tempPath) + QDir::separator();
    QDir(tempPath).mkdir("backup_2");

    QSqlDatabase storageDb = DatabaseRegistry::fileStorageDatabase();

    V2_FileStorageManager v2fsm(storageDb, tempPath + "backup_2");
    v2fsm.addNewFolder("/", "test_folder/", "/home/user/Desktop/data");
    v2fsm.addNewFile("/test_folder/","/home/user/Desktop/data/text_file.txt");
    v2fsm.appendVersion("/test_folder/text_file.txt", "/home/user/Desktop/data/text_file.txt", "second version");

    QJsonObject rootFolderJson = v2fsm.getFolderJson("/", true);
    QJsonObject childFolderJson = v2fsm.getFolderJson("/test_folder/", true);

    return app.exec();
}
