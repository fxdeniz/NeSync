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
    httpServer.route("/folder/add", QHttpServerRequest::Method::Post, [&storageController](const QHttpServerRequest &request) {
        return storageController.addNewFolder(request);
    });

    httpServer.route("/folder/get", QHttpServerRequest::Method::Post, [&storageController](const QHttpServerRequest &request) {
        return storageController.getFolder(request);
    });

    httpServer.route("/folder/getByUserPath", QHttpServerRequest::Method::Post, [&storageController](const QHttpServerRequest &request) {
        return storageController.getFolderUserPath(request);
    });

    httpServer.route("/folder/storageFolderPath", QHttpServerRequest::Method::Get, [&storageController](const QHttpServerRequest &request) {
        return storageController.getStorageFolderPath(request);
    });

    httpServer.route("/folder/delete", QHttpServerRequest::Method::Delete, [&storageController](const QHttpServerRequest &request) {
        return storageController.deleteFolder(request);
    });

    httpServer.route("/file/add", QHttpServerRequest::Method::Post, [&storageController](const QHttpServerRequest &request) {
        return storageController.addNewFile(request);
    });

    httpServer.route("/file/get", QHttpServerRequest::Method::Post, [&storageController](const QHttpServerRequest &request) {
        return storageController.getFile(request);
    });

    httpServer.route("/file/getByUserPath", QHttpServerRequest::Method::Post, [&storageController](const QHttpServerRequest &request) {
        return storageController.getFileByUserPath(request);
    });

    httpServer.route("/file/update/name", QHttpServerRequest::Method::Post, [&storageController](const QHttpServerRequest &request) {
        return storageController.renameFile(request);
    });

    httpServer.route("/file/update/frozenStatus", QHttpServerRequest::Method::Post, [&storageController](const QHttpServerRequest &request) {
        return storageController.updateFileFrozenStatus(request);
    });

    httpServer.route("/file/append", QHttpServerRequest::Method::Post, [&storageController](const QHttpServerRequest &request) {
        return storageController.appendVersion(request);
    });

    httpServer.route("/file/delete", QHttpServerRequest::Method::Delete, [&storageController](const QHttpServerRequest &request) {
        return storageController.deleteFile(request);
    });

    httpServer.route("/version/update/description", QHttpServerRequest::Method::Post, [&storageController](const QHttpServerRequest &request) {
        return storageController.updateFileVersionDescription(request);
    });

    httpServer.route("/monitor/new", QHttpServerRequest::Method::Get, [&fsMonitorController](const QHttpServerRequest &request) {
        return fsMonitorController.newAddedItems(request);
    });

    httpServer.route("/monitor/deleted", QHttpServerRequest::Method::Get, [&fsMonitorController](const QHttpServerRequest &request) {
        return fsMonitorController.deletedItems(request);
    });

    httpServer.route("/monitor/updated", QHttpServerRequest::Method::Get, [&fsMonitorController](const QHttpServerRequest &request) {
        return fsMonitorController.updatedFiles(request);
    });

    httpServer.route("/export/zip/setFilePath", QHttpServerRequest::Method::Post, [&zipExportController](const QHttpServerRequest &request) {
        return zipExportController.setFilePath(request);
    });

    httpServer.route("/export/zip/getFilePath", QHttpServerRequest::Method::Get, [&zipExportController](const QHttpServerRequest &request) {
        return zipExportController.getFilePath(request);
    });

    httpServer.route("/export/zip/setRootFolder", QHttpServerRequest::Method::Post, [&zipExportController](const QHttpServerRequest &request) {
        return zipExportController.setRootFolder(request);
    });

    httpServer.route("export/zip/getRootFolder", QHttpServerRequest::Method::Get, [&zipExportController](const QHttpServerRequest &request) {
        return zipExportController.getRootFolder(request);
    });

    httpServer.route("/export/zip/create", QHttpServerRequest::Method::Post, [&zipExportController](const QHttpServerRequest &request) {
        return zipExportController.createZip(request);
    });

    httpServer.route("/export/zip/addFoldersJson", QHttpServerRequest::Method::Post, [&zipExportController](const QHttpServerRequest &request) {
        return zipExportController.addFoldersJson(request);
    });

    httpServer.route("/export/zip/addFilesJson", QHttpServerRequest::Method::Post, [&zipExportController](const QHttpServerRequest &request) {
        return zipExportController.addFilesJson(request);
    });

    httpServer.route("/export/zip/addFile", QHttpServerRequest::Method::Post, [&zipExportController](const QHttpServerRequest &request) {
        return zipExportController.addFile(request);
    });

    httpServer.route("/import/zip/setFilePath", QHttpServerRequest::Method::Post, [&zipImportController](const QHttpServerRequest &request) {
        return zipImportController.setFilePath(request);
    });

    httpServer.route("/import/zip/getFilePath", QHttpServerRequest::Method::Get, [&zipImportController](const QHttpServerRequest &request) {
        return zipImportController.getFilePath(request);
    });

    httpServer.route("/import/zip/open", QHttpServerRequest::Method::Get, [&zipImportController](const QHttpServerRequest &request) {
        return zipImportController.openZip(request);
    });

    httpServer.route("/import/zip/readFoldersJson", QHttpServerRequest::Method::Get, [&zipImportController](const QHttpServerRequest &request) {
        return zipImportController.readFoldersJson(request);
    });

    httpServer.route("/import/zip/readFilesJson", QHttpServerRequest::Method::Get, [&zipImportController](const QHttpServerRequest &request) {
        return zipImportController.readFilesJson(request);
    });

    httpServer.route("/import/zip/importFileFromZip", QHttpServerRequest::Method::Post, [&zipImportController](const QHttpServerRequest &request) {
        return zipImportController.importFileFromZip(request);
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
