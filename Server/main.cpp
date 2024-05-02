#include <QCoreApplication>
#include <QDir>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QStandardPaths>
#include <QtHttpServer/QHttpServer>
#include <QtHttpServer/QHttpServerResponse>

#include "Utility/AppConfig.h"
#include "Utility/JsonDtoFormat.h"
#include "RestApi/RestController.h"
#include "FileStorageSubSystem/FileStorageManager.h"
#include "FileMonitorSubSystem/FileMonitoringManager.h"

QHttpServerResponse postAddNewFile(const QHttpServerRequest& request)
{
    QHttpServerResponse response(QHttpServerResponse::StatusCode::NotImplemented);
    QByteArray requestBody = request.body();

    if(requestBody.isEmpty())
    {
        QString errorMessage = "Body is empty";
        response = QHttpServerResponse(errorMessage, QHttpServerResponse::StatusCode::BadRequest);
        return response;
    }


    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(requestBody, &jsonError);

    if(jsonError.error != QJsonParseError::NoError)
    {
        QString errorMessage = "Input format is not parsable json.";
        response = QHttpServerResponse(errorMessage, QHttpServerResponse::StatusCode::BadRequest);
        return response;
    }

    if(!jsonDoc.isObject())
    {
        QString errorMessage = "Input json is not an object.";
        response = QHttpServerResponse(errorMessage, QHttpServerResponse::StatusCode::BadRequest);
        return response;
    }

    QJsonObject jsonObject = jsonDoc.object();

    QString symbolFolderPath = jsonObject["symbolFolderPath"].toString();
    QString pathToFile = jsonObject["pathToFile"].toString();
    QString description = jsonObject["description"].toString();
    bool isFrozen = jsonObject["isFrozen"].toBool();

    qDebug() << "symbolFolderPath = " << symbolFolderPath;
    qDebug() << "pathToFile = " << pathToFile;
    qDebug() << "description = " << description;
    qDebug() << "isFrozen = " << isFrozen;

    auto fsm = FileStorageManager::instance();

    QJsonObject folderJson = fsm->getFolderJsonBySymbolPath(symbolFolderPath);

    if(folderJson[JsonKeys::IsExist].toBool())
    {
        bool isAdded = fsm->addNewFile(symbolFolderPath, pathToFile, isFrozen, "", description);

        if(isAdded)
        {
            QString reponseMessage = "File is created.";
            response = QHttpServerResponse(reponseMessage, QHttpServerResponse::StatusCode::Created);
            return response;
        }
    }

    return response;
}

QHttpServerResponse postAppendVersion(const QHttpServerRequest& request)
{
    QHttpServerResponse response(QHttpServerResponse::StatusCode::NotImplemented);
    QByteArray requestBody = request.body();

    if(requestBody.isEmpty())
    {
        QString errorMessage = "Body is empty";
        response = QHttpServerResponse(errorMessage, QHttpServerResponse::StatusCode::BadRequest);
        return response;
    }


    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(requestBody, &jsonError);

    if(jsonError.error != QJsonParseError::NoError)
    {
        QString errorMessage = "Input format is not parsable json.";
        response = QHttpServerResponse(errorMessage, QHttpServerResponse::StatusCode::BadRequest);
        return response;
    }

    if(!jsonDoc.isObject())
    {
        QString errorMessage = "Input json is not an object.";
        response = QHttpServerResponse(errorMessage, QHttpServerResponse::StatusCode::BadRequest);
        return response;
    }

    QJsonObject jsonObject = jsonDoc.object();

    QString pathToFile = jsonObject["pathToFile"].toString();
    QString description = jsonObject["description"].toString();

    qDebug() << "pathToFile = " << pathToFile;
    qDebug() << "description = " << description;

    auto fsm = FileStorageManager::instance();

    QJsonObject fileJson = fsm->getFileJsonByUserPath(pathToFile);

    if(fileJson[JsonKeys::IsExist].toBool())
    {
        bool isAppended = fsm->appendVersion(fileJson[JsonKeys::File::SymbolFilePath].toString(), pathToFile, description);

        if(isAppended)
        {
            QString reponseMessage = "Version appended.";
            response = QHttpServerResponse(reponseMessage, QHttpServerResponse::StatusCode::Ok);
            return response;
        }
    }

    return response;
}


QHttpServerResponse getFolderContent(const QHttpServerRequest& request)
{
    QString symbolFolderPath = request.query().queryItemValue("symbolPath");
    qDebug() << "symbolFolderPath = " << symbolFolderPath;

    auto fsm = FileStorageManager::instance();
    QJsonObject responseBody = fsm->getFolderJsonBySymbolPath(symbolFolderPath, true);

    QHttpServerResponse response(responseBody);
    response.addHeader("Access-Control-Allow-Origin", "*");
    return response;
}

QHttpServerResponse startMonitoring(QThread *fileMonitorThread, FileSystemEventStore *fses, const QHttpServerRequest& request)
{
    fses->clear();

    QJsonObject responseBody;
    FileStorageManager *fsm = FileStorageManager::rawInstance();

    FileMonitoringManager *fmm = new FileMonitoringManager(fsm, fses);

    for(const QJsonValue &value : fsm->getActiveFolderList())
    {
        QJsonObject folderJson = value.toObject();

        QString symbolFolderPath = folderJson[JsonKeys::Folder::SymbolFolderPath].toString();
        QString userFolderPath = folderJson[JsonKeys::Folder::UserFolderPath].toString();

        fmm->addFolder(userFolderPath);
    }


    // TODO: Do the signal slot connections for fmm here
    QObject::connect(fileMonitorThread, &QThread::finished, fmm, &QObject::deleteLater);

    fmm->moveToThread(fileMonitorThread);
    fileMonitorThread->start();

    QHttpServerResponse response(responseBody, QHttpServerResponse::StatusCode::Ok);
    response.addHeader("Access-Control-Allow-Origin", "*");
    return response;
}

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

    httpServer.route("/addNewFile", QHttpServerRequest::Method::Post, [](const QHttpServerRequest &request) {
        return postAddNewFile(request);
    });

    httpServer.route("/appendVersion", QHttpServerRequest::Method::Post, [](const QHttpServerRequest &request) {
        return postAppendVersion(request);
    });

    httpServer.route("/getFolderContent", QHttpServerRequest::Method::Get, [](const QHttpServerRequest &request) {
        return getFolderContent(request);
    });

    httpServer.route("/startMonitoring", QHttpServerRequest::Method::Get, [&fileMonitorThread, fses](const QHttpServerRequest &request) {

        if(fileMonitorThread != nullptr)
        {
            fileMonitorThread->quit();
            fileMonitorThread->wait();
            delete fileMonitorThread;
            fileMonitorThread = nullptr;
        }

        fileMonitorThread = new QThread();

        return startMonitoring(fileMonitorThread, fses, request);
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
