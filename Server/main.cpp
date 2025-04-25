#include <QCoreApplication>
#include <QDir>
#include <QDebug>
#include <QTcpServer>
#include <QJsonObject>
#include <QStorageInfo>
#include <QLibraryInfo>
#include <QJsonDocument>
#include <QStandardPaths>
#include <QCommandLineParser>
#include <QProcessEnvironment>
#include <QtHttpServer/QHttpServer>
#include <QtHttpServer/QHttpServerResponse>

#include "Utility/AppConfig.h"
#include "RestApi/FileStorageController.h"
#include "RestApi/ZipExportController.h"
#include "RestApi/ZipImportController.h"
#include "RestApi/FileSystemMonitorController.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    qInfo() << "NeSync Server 2.0 Alpha - Copyright 2025 Deniz Yilmazok - Licensed under GPLv3";
    qInfo() << "THIS SOFTWARE COMES WITH ABSOLUTELY NO WARRANTY! USE AT YOUR OWN RISK!";

    QCommandLineParser parser;
    parser.setApplicationDescription("NeSync 2.0");
    parser.addHelpOption();
    parser.addVersionOption();

    QString storagePath = QStandardPaths::writableLocation(QStandardPaths::StandardLocation::HomeLocation);
    storagePath = QDir::toNativeSeparators(storagePath) + QDir::separator();
    storagePath += "nesync_server";
    storagePath += QDir::separator();

    QString pathMessage = "Specify the storage path (default: %1).";
    pathMessage = pathMessage.arg(storagePath);

    QCommandLineOption optionPort(QStringList() << "p" << "port",
                                  "Specify the port number to use (default: 0).",
                                  "number", 0);

    QCommandLineOption optionPath(QStringList() << "d" << "dir",
                                  pathMessage,
                                  "path", storagePath);

    parser.addOption(optionPort);
    parser.addOption(optionPath);
    parser.process(app);

    bool isPortSet;
    int portNumber = parser.value(optionPort).toInt(&isPortSet);
    storagePath = parser.value(optionPath);

    if (!isPortSet || portNumber <= 0 || portNumber > 65535)
    {
        qCritical() << "Invalid or missing port number.";
        return 10;
    }

    QDir dir(storagePath);
    bool isPathValid = dir.mkpath(storagePath);

    if(!isPathValid)
    {
        qCritical() << "Invalid storage location.";
        return 11;
    }

    AppConfig::setStorageFolderPath(storagePath);

    QTcpServer tcpServer;
    QHttpServer httpServer;
    FileStorageController storageController;
    FileSystemMonitorController fsMonitorController;
    ZipExportController zipExportController;
    ZipImportController zipImportController;

    httpServer.route("/", QHttpServerRequest::Method::Get, [portNumber, storagePath](const QHttpServerRequest &request) {
        QStorageInfo storageInfo(QCoreApplication::applicationDirPath());
        QFileInfoList deviceInfoList = QDir::drives().toList();
        QStringList devices;

        for(const QFileInfo &info : deviceInfoList)
            devices << QString(QStorageInfo(info.absolutePath()).device());

        QJsonObject body = {
            {"appVersion", "2.0"},
            {"port", portNumber},
            {"storageFolderPath", storagePath},
            {"cwd", QCoreApplication::applicationFilePath()},
            {"cwdParent", QCoreApplication::applicationDirPath()},
            {"pid", QCoreApplication::applicationPid()},
            {"appBinaryName", QCoreApplication::applicationName()},
            {"appBinaryVersion", QCoreApplication::applicationVersion()},
            {"arguments", QJsonArray::fromStringList(QCoreApplication::arguments())},
            {"dynamicLibPaths", QJsonArray::fromStringList(QCoreApplication::libraryPaths())},
            {"hostName", QSysInfo::machineHostName()},
            {"osName", QSysInfo::prettyProductName()},
            {"osVersion", QSysInfo::productVersion()},
            {"osType", QSysInfo::productType()},
            {"kernelType", QSysInfo::kernelType()},
            {"kernelVersion", QSysInfo::kernelVersion()},
            {"currentCpuArch", QSysInfo::currentCpuArchitecture()},
            {"buildCpuArch", QSysInfo::buildCpuArchitecture()},
            {"buildAbi", QSysInfo::buildAbi()},
            {"qtVersion", QLibraryInfo::version().toString()},
            {"isDebugBuild", QLibraryInfo::isDebugBuild()},
            {"isQtSharedBuild", QLibraryInfo::isSharedBuild()},
            {"storageDevice", QString(storageInfo.device())},
            {"storageDeviceName", storageInfo.displayName()},
            {"storageDeviceCapacityTotal", storageInfo.bytesTotal()},
            {"storageDeviceCapacityFree", storageInfo.bytesFree()},
            {"storageDeviceCapacityAvailable", storageInfo.bytesAvailable()},
            {"storageDeviceFileSystem", QString(storageInfo.fileSystemType())},
            {"storageDeviceRoot", storageInfo.rootPath()},
            {"storageDevices", {QJsonArray::fromStringList(devices)}},
            {"environment", QJsonArray::fromStringList(QProcessEnvironment::systemEnvironment().toStringList())},
            {"timestamp", QDateTime::currentDateTime().toString(Qt::DateFormat::ISODateWithMs)}
        };

        QHttpServerResponse response(body);
        return response;
    });

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

    httpServer.route("/folder/rename", QHttpServerRequest::Method::Post, [&storageController](const QHttpServerRequest &request) {
        return storageController.renameFolder(request);
    });

    httpServer.route("/folder/freeze", QHttpServerRequest::Method::Post, [&storageController](const QHttpServerRequest &request) {
        return storageController.freezeFolder(request);
    });

    httpServer.route("/folder/relocate", QHttpServerRequest::Method::Post, [&storageController](const QHttpServerRequest &request) {
        return storageController.relocateFolder(request);
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

    httpServer.route("/file/update/freeze", QHttpServerRequest::Method::Post, [&storageController](const QHttpServerRequest &request) {
        return storageController.freezeFile(request);
    });

    httpServer.route("/file/append", QHttpServerRequest::Method::Post, [&storageController](const QHttpServerRequest &request) {
        return storageController.appendVersion(request);
    });

    httpServer.route("/file/relocate", QHttpServerRequest::Method::Post, [&storageController](const QHttpServerRequest &request) {
        return storageController.relocateFile(request);
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

    tcpServer.listen(QHostAddress::SpecialAddress::LocalHost, portNumber);

    if (tcpServer.isListening() && httpServer.bind(&tcpServer))
        qDebug() << "running on = " << "localhost:" + QString::number(portNumber);
    else
    {
        qWarning() << QCoreApplication::translate("QHttpServerExample",
                                                  "Server failed to listen on a port.");
        return 12;
    }

    return app.exec();
}
