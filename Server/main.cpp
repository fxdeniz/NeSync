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

    httpServer.route("/newAddedList", QHttpServerRequest::Method::Get, [&restController](const QHttpServerRequest &request) {
        return restController.newAddedList(request);
    });

    httpServer.route("/deletedList", QHttpServerRequest::Method::Get, [&restController](const QHttpServerRequest &request) {
        return restController.deletedList(request);
    });

    httpServer.route("/updatedFileList", QHttpServerRequest::Method::Get, [&restController](const QHttpServerRequest &request) {
        return restController.updatedFileList(request);
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
