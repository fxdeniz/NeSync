#include <QCoreApplication>
#include <QDir>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QStandardPaths>
#include <QtHttpServer/QHttpServer>
#include <QtHttpServer/QHttpServerResponse>

#include "Utility/AppConfig.h"
#include "RestApi/RestController.h"

QHttpServerResponse dumpFses(FileSystemEventStore *fses, const QHttpServerRequest& request)
{
    QJsonObject responseBody;

    for(const QString &currentFolderPath : fses->folderList())
    {
        responseBody.insert(currentFolderPath, fses->statusOfFolder(currentFolderPath));
    }

    QHttpServerResponse response(responseBody, QHttpServerResponse::StatusCode::Ok);
    response.addHeader("Access-Control-Allow-Origin", "*");
    return response;
}


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

    QThread *fileMonitorThread = nullptr;
    FileSystemEventStore *fses = new FileSystemEventStore();

    QHttpServer httpServer;
    RestController restController;

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

    httpServer.route("/getFolderContent", QHttpServerRequest::Method::Get, [&restController](const QHttpServerRequest &request) {
        return restController.getFolderContent(request);
    });

    httpServer.route("/startMonitoring", QHttpServerRequest::Method::Get, [&restController](const QHttpServerRequest &request) {
        return restController.startMonitoring(request);
    });

    httpServer.route("/dumpFses", QHttpServerRequest::Method::Get, [fses](const QHttpServerRequest &request) {
        return dumpFses(fses, request);
    });

    quint16 targetPort = 1234; // Making this 0, means random port.
    quint16 port = httpServer.listen(QHostAddress::SpecialAddress::Any, targetPort);
    if (port)
        qDebug() << "running on = " << "localhost:" + QString::number(port);
    else
    {
        qWarning() << QCoreApplication::translate("QHttpServerExample",
                                                  "Server failed to listen on a port.");
        return -1;
    }

    return a.exec();
}
