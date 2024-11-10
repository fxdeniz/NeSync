#include <QCoreApplication>
#include <QDir>
#include <QDebug>
#include <QTcpServer>
#include <QJsonObject>
#include <QJsonDocument>
#include <QStandardPaths>
#include <QtHttpServer/QHttpServer>
#include <QtHttpServer/QHttpServerResponse>

#include "Utility/AppConfig.h"
#include "RestApi/FileStorageController.h"
#include "RestApi/ZipExportController.h"
#include "RestApi/ZipImportController.h"
#include "RestApi/FileSystemMonitorController.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString storagePath = QStandardPaths::writableLocation(QStandardPaths::StandardLocation::HomeLocation);
    storagePath = QDir::toNativeSeparators(storagePath) + QDir::separator();
    storagePath += "nesync_server_";
    storagePath += QUuid::createUuid().toString(QUuid::StringFormat::WithoutBraces).mid(0, 8);
    storagePath += QDir::separator();

    QDir().mkpath(storagePath);
    AppConfig().setStorageFolderPath(storagePath);

    QTcpServer tcpServer;
    QHttpServer httpServer;
    FileStorageController storageController;
    FileSystemMonitorController fsMonitorController;
    ZipExportController zipExportController;
    ZipImportController zipImportController;

    // For routing checkout: https://www.qt.io/blog/2019/02/01/qhttpserver-routing-api
    httpServer.route("/addNewFolder", QHttpServerRequest::Method::Post, [&storageController](const QHttpServerRequest &request) {
        return storageController.postAddNewFolder(request);
    });

    httpServer.route("/addNewFile", QHttpServerRequest::Method::Post, [&storageController](const QHttpServerRequest &request) {
        return storageController.postAddNewFile(request);
    });

    httpServer.route("/appendVersion", QHttpServerRequest::Method::Post, [&storageController](const QHttpServerRequest &request) {
        return storageController.postAppendVersion(request);
    });

    httpServer.route("/deleteFolder", QHttpServerRequest::Method::Delete, [&storageController](const QHttpServerRequest &request) {
        return storageController.deleteFolder(request);
    });

    httpServer.route("/deleteFile", QHttpServerRequest::Method::Delete, [&storageController](const QHttpServerRequest &request) {
        return storageController.deleteFile(request);
    });

    httpServer.route("/getFolderContent", QHttpServerRequest::Method::Get, [&storageController](const QHttpServerRequest &request) {
        return storageController.getFolderContent(request);
    });

    httpServer.route("/getFolderContentByUserPath", QHttpServerRequest::Method::Get, [&storageController](const QHttpServerRequest &request) {
        return storageController.getFolderContentByUserPath(request);
    });

    httpServer.route("/getFileContentByUserPath", QHttpServerRequest::Method::Get, [&storageController](const QHttpServerRequest &request) {
        return storageController.getFileContentByUserPath(request);
    });

    httpServer.route("/newAddedList", QHttpServerRequest::Method::Get, [&fsMonitorController](const QHttpServerRequest &request) {
        return fsMonitorController.newAddedItems(request);
    });

    httpServer.route("/deletedList", QHttpServerRequest::Method::Get, [&fsMonitorController](const QHttpServerRequest &request) {
        return fsMonitorController.deletedItems(request);
    });

    httpServer.route("/updatedFileList", QHttpServerRequest::Method::Get, [&fsMonitorController](const QHttpServerRequest &request) {
        return fsMonitorController.updatedFiles(request);
    });

    httpServer.route("/postSetZipFilePath", QHttpServerRequest::Method::Post, [&zipExportController](const QHttpServerRequest &request) {
        return zipExportController.postSetZipFilePath(request);
    });

    httpServer.route("/getZipFilePath", QHttpServerRequest::Method::Get, [&zipExportController](const QHttpServerRequest &request) {
        return zipExportController.getZipFilePath(request);
    });

    httpServer.route("/postSetRootSymbolFolderPath", QHttpServerRequest::Method::Post, [&zipExportController](const QHttpServerRequest &request) {
        return zipExportController.postSetRootSymbolFolderPath(request);
    });

    httpServer.route("/getRootSymbolFolderPath", QHttpServerRequest::Method::Get, [&zipExportController](const QHttpServerRequest &request) {
        return zipExportController.getRootSymbolFolderPath(request);
    });

    httpServer.route("/postCreateZipArchive", QHttpServerRequest::Method::Post, [&zipExportController](const QHttpServerRequest &request) {
        return zipExportController.postCreateArchive(request);
    });

    httpServer.route("/postAddFolderJson", QHttpServerRequest::Method::Post, [&zipExportController](const QHttpServerRequest &request) {
        return zipExportController.postAddFoldersJson(request);
    });

    httpServer.route("/postAddFileJson", QHttpServerRequest::Method::Post, [&zipExportController](const QHttpServerRequest &request) {
        return zipExportController.postAddFileJson(request);
    });

    httpServer.route("/postAddFileToZip", QHttpServerRequest::Method::Post, [&zipExportController](const QHttpServerRequest &request) {
        return zipExportController.postAddFileToZip(request);
    });

    httpServer.route("/zip/import/ZipFilePath", QHttpServerRequest::Method::Post, [&zipImportController](const QHttpServerRequest &request) {
        return zipImportController.setZipFilePath(request);
    });

    httpServer.route("/zip/import/ZipFilePath", QHttpServerRequest::Method::Get, [&zipImportController](const QHttpServerRequest &request) {
        return zipImportController.getZipFilePath(request);
    });

    httpServer.route("/zip/import/OpenFile", QHttpServerRequest::Method::Get, [&zipImportController](const QHttpServerRequest &request) {
        return zipImportController.openArchive(request);
    });

    httpServer.route("/zip/import/ReadFoldersJson", QHttpServerRequest::Method::Get, [&zipImportController](const QHttpServerRequest &request) {
        return zipImportController.readFoldersJson(request);
    });

    httpServer.route("/zip/import/ReadFilesJson", QHttpServerRequest::Method::Get, [&zipImportController](const QHttpServerRequest &request) {
        return zipImportController.readFilesJson(request);
    });

    httpServer.route("/zip/import/FileVersion", QHttpServerRequest::Method::Post, [&zipImportController](const QHttpServerRequest &request) {
        return zipImportController.importFileVersion(request);
    });

    quint16 targetPort = 1234; // Making this 0, means random port.
    tcpServer.listen(QHostAddress::SpecialAddress::LocalHost, targetPort);

    if (tcpServer.isListening() && httpServer.bind(&tcpServer))
        qDebug() << "running on = " << "localhost:" + QString::number(targetPort);
    else
    {
        qWarning() << QCoreApplication::translate("QHttpServerExample",
                                                  "Server failed to listen on a port.");
        return -1;
    }

    return a.exec();
}
