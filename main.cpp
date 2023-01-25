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

    FolderRepository folderRepo(storageDb);

    FolderEntity parent, child;
    parent.suffixPath = "/";

    child.parentFolderPath = parent.symbolFolderPath();
    child.suffixPath = "Desktop/";
    child.userFolderPath = "/home/user/Desktop/";
    child.isFrozen = true;

    folderRepo.save(parent);
    folderRepo.save(child);

    FolderEntity childResult = folderRepo.findBySymbolPath("/Desktop/");
    qDebug() << "symbol path = " << childResult.symbolFolderPath();

    FileEntity fileEntity;
    fileEntity.fileName = "text_file.txt";
    fileEntity.symbolFolderPath = childResult.symbolFolderPath();
    fileEntity.isFrozen = true;

    FileEntity fileToRoot;
    fileToRoot.fileName = "sample_file.pdf";
    fileToRoot.symbolFolderPath = parent.symbolFolderPath();
    fileToRoot.isFrozen = true;

    FileRepository fileRepo(storageDb);
    fileRepo.save(fileEntity);
    fileRepo.save(fileToRoot);

    FolderEntity fatFolder = folderRepo.findBySymbolPath("/", true);

    FileEntity fileResult = fileRepo.findBySymbolPath(fileEntity.symbolFilePath());

    FileVersionEntity version;
    version.symbolFilePath = fileResult.symbolFilePath();
    version.versionNumber = 4;
    version.internalFileName = "file_name_here";
    version.size = 100;
    version.description = "description here";
    version.hash = "a1b2c3d4e5";

    FileVersionRepository versionRepo(storageDb);
    versionRepo.save(version);

    FileVersionEntity versionResult = versionRepo.findVersion(fileResult.symbolFilePath(), 4);

    return app.exec();
}
