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
#include "RestApi/RestController.h"
#include "RestApi/ZipExportController.h"

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
    RestController restController;
    ZipExportController zipExportController;

    // For routing checkout: https://www.qt.io/blog/2019/02/01/qhttpserver-routing-api
    httpServer.route("/addNewFolder", QHttpServerRequest::Method::Post, [&restController](const QHttpServerRequest &request) {
        return restController.postAddNewFolder(request);
    });

    httpServer.route("/addNewFile", QHttpServerRequest::Method::Post, [&restController](const QHttpServerRequest &request) {
        return restController.postAddNewFile(request);
    });

    httpServer.route("/appendVersion", QHttpServerRequest::Method::Post, [&restController](const QHttpServerRequest &request) {
        return restController.postAppendVersion(request);
    });

    httpServer.route("/deleteFolder", QHttpServerRequest::Method::Delete, [&restController](const QHttpServerRequest &request) {
        return restController.deleteFolder(request);
    });

    httpServer.route("/deleteFile", QHttpServerRequest::Method::Delete, [&restController](const QHttpServerRequest &request) {
        return restController.deleteFile(request);
    });

    httpServer.route("/getFolderContent", QHttpServerRequest::Method::Get, [&restController](const QHttpServerRequest &request) {
        return restController.getFolderContent(request);
    });

    httpServer.route("/getFolderContentByUserPath", QHttpServerRequest::Method::Get, [&restController](const QHttpServerRequest &request) {
        return restController.getFolderContentByUserPath(request);
    });

    httpServer.route("/getFileContentByUserPath", QHttpServerRequest::Method::Get, [&restController](const QHttpServerRequest &request) {
        return restController.getFileContentByUserPath(request);
    });

    httpServer.route("/newAddedList", QHttpServerRequest::Method::Get, [&restController](const QHttpServerRequest &request) {
        return restController.newAddedList(request);
    });

    httpServer.route("/deletedList", QHttpServerRequest::Method::Get, [&restController](const QHttpServerRequest &request) {
        return restController.deletedList(request);
    });

    httpServer.route("/updatedFileList", QHttpServerRequest::Method::Get, [&restController](const QHttpServerRequest &request) {
        return restController.updatedFileList(request);
    });

    httpServer.route("/simpleNewAddedList", QHttpServerRequest::Method::Get, [&restController](const QHttpServerRequest &request) {
        //return restController.simpleNewAddedList(request);
        return restController.simpleNewAddedList_V2(request);
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
        return zipExportController.postAddFolderJson(request);
    });

    httpServer.route("/postAddFileJson", QHttpServerRequest::Method::Post, [&zipExportController](const QHttpServerRequest &request) {
        return zipExportController.postAddFileJson(request);
    });

    httpServer.route("/postAddFileToZip", QHttpServerRequest::Method::Post, [&zipExportController](const QHttpServerRequest &request) {
        return zipExportController.postAddFileToZip(request);
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
