#include <QStandardPaths>
#include <QApplication>
#include <QDebug>
#include <QJsonArray>

#include "Gui/MainWindow.h"
#include "Backend/FileMonitorSubSystem/FileMonitoringManager.h"
#include "Backend/FileMonitorSubSystem/FileSystemEventDb.h"
#include "Utility/DatabaseRegistry.h"
#include "FileStorageSubSystem/ORM/Repository/FolderRepository.h"
#include "FileStorageSubSystem/ORM/Repository/FileRepository.h"
#include "FileStorageSubSystem/ORM/Repository/FileVersionRepository.h"
#include "FileStorageSubSystem/FileStorageManager.h"

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

//    QString tempPath = QStandardPaths::writableLocation(QStandardPaths::StandardLocation::TempLocation);
//    tempPath = QDir::toNativeSeparators(tempPath) + QDir::separator();
//    QDir(tempPath).mkdir("backup_2");

//    QSqlDatabase storageDb = DatabaseRegistry::fileStorageDatabase();

//    auto v2fsm = FileStorageManager::instance();
//    v2fsm->addNewFolder("/test_folder/", "/home/user/Desktop/data/");
//    v2fsm->addNewFile("/test_folder/","/home/user/Desktop/data/text_file.txt");
//    v2fsm->appendVersion("/test_folder/text_file.txt", "/home/user/Desktop/data/text_file.txt", "second version");
//    v2fsm->appendVersion("/test_folder/text_file.txt", "/home/user/Desktop/data/text_file.txt", "third version");

//    v2fsm->deleteFile("/test_folder/text_file.txt");

//    QJsonObject rootFolderJson = v2fsm->getFolderJsonBySymbolPath("/", true);
//    QJsonObject childFolderJson = v2fsm->getFolderJsonBySymbolPath("/test_folder/", true);
//    QJsonObject fileJson = v2fsm->getFileJsonBySymbolPath("/test_folder/text_file.txt", true);
//    QJsonObject versionJson = v2fsm->getFileVersionJson("/test_folder/text_file.txt", 2);
//    QJsonObject fileJsonByUserPath = v2fsm->getFileJsonByUserPath("/home/user/Desktop/data/text_file.txt", true);
//    qDebug() << V2_FileStorageManager::instance()->getActiveFolderList();

    return app.exec();
}
