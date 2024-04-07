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
#include "FileStorageSubSystem/FileStorageManager.h"
#include "FileMonitorSubSystem/FileMonitoringManager.h"

// For routing checkout: https://www.qt.io/blog/2019/02/01/qhttpserver-routing-api

QHttpServerResponse postAddNewFolder(const QHttpServerRequest& request)
{
    QHttpServerResponse response(QHttpServerResponse::StatusCode::NotImplemented);
    response.addHeader("Access-Control-Allow-Origin", "*");
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
    QString userFolderPath = jsonObject["userFolderPath"].toString();

    qDebug() << "symbolFolderPath = " << symbolFolderPath;
    qDebug() << "userFolderPath = " << userFolderPath;

    auto fsm = FileStorageManager::instance();
    bool isAdded = fsm->addNewFolder(symbolFolderPath, userFolderPath);

    if(isAdded)
    {
        QString reponseMessage = "Folder is created.";
        QHttpServerResponse response = QHttpServerResponse(reponseMessage, QHttpServerResponse::StatusCode::Created);
        response.addHeader("Access-Control-Allow-Origin", "*");

        return response;        
    }

    return response;
}

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

QHttpServerResponse postStartMonitoring(FileMonitoringManager &fmm, const QHttpServerRequest& request)
{
    auto fsm = FileStorageManager::instance();

    for(const QJsonValue &value : fsm->getActiveFolderList())
    {
        QJsonObject folderJson = value.toObject();

        QString symbolFolderPath = folderJson[JsonKeys::Folder::SymbolFolderPath].toString();
        QString userFolderPath = folderJson[JsonKeys::Folder::UserFolderPath].toString();

        fmm.addFolder(userFolderPath);

        folderJson = fsm->getFolderJsonBySymbolPath(symbolFolderPath, true);

        for(const QJsonValue &value : folderJson[JsonKeys::Folder::ChildFiles].toArray())
        {
            QJsonObject fileJson = value.toObject();
            QString fileName = fileJson[JsonKeys::File::FileName].toString();

            fmm.addFile(userFolderPath, fileName);
        }
    }

    QString reponseMessage = "Monitoring started.";
    return QHttpServerResponse(reponseMessage, QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse patchSetStatusOfMonitoredFile(FileSystemEventDb *fsEventDb, const QHttpServerRequest& request)
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
    FileSystemEventDb::ItemStatus status = FileSystemEventDb::toItemStatus(jsonObject["status"].toInteger());

    qDebug() << "pathToFile = " << pathToFile;
    qDebug() << "status = " << status;

    if(status == FileSystemEventDb::ItemStatus::Invalid)
    {
        QString errorMessage = "Status is invalid.";
        response = QHttpServerResponse(errorMessage, QHttpServerResponse::StatusCode::BadRequest);
        return response;
    }

    auto fsm = FileStorageManager::instance();

    QJsonObject fileJson = fsm->getFileJsonByUserPath(pathToFile);
    QJsonObject folderJson = fsm->getFolderJsonBySymbolPath(fileJson[JsonKeys::File::SymbolFolderPath].toString());

    QString userFolderPath = folderJson[JsonKeys::Folder::UserFolderPath].toString();
    QString fileName = fileJson[JsonKeys::File::FileName].toString();

    bool isFileMonitored = fsEventDb->isMonitoredFileExist(userFolderPath, fileName);

    if(!isFileMonitored)
    {
        QString errorMessage = "File is not monitored.";
        response = QHttpServerResponse(errorMessage, QHttpServerResponse::StatusCode::BadRequest);
        return response;
    }

    if(fileJson[JsonKeys::IsExist].toBool())
    {
        bool isUpdated = fsEventDb->setStatusOfMonitoredFile(userFolderPath, fileName, status);

        if(isUpdated)
        {
            QString reponseMessage = "File status updated.";
            response = QHttpServerResponse(reponseMessage, QHttpServerResponse::StatusCode::Ok);
            return response;
        }
    }

    return response;
}

QHttpServerResponse getEventsOnFolderTree(FileSystemEventDb *fsEventDb)
{
    QHash<FileSystemEventDb::ItemStatus, QStringList> folderQueryResult = fsEventDb->getEventsOnMonitoredFolders();
    QHash<FileSystemEventDb::ItemStatus, QStringList> fileQueryResult = fsEventDb->getEventsOnMonitoredFiles();

    auto status = FileSystemEventDb::ItemStatus::Updated;
    QStringList updatedFileList = fileQueryResult.value(status);

    status = FileSystemEventDb::ItemStatus::Deleted;
    QStringList deletedFolderList = folderQueryResult.value(status);
    QStringList deletedFileList = fileQueryResult.value(status);

    status = FileSystemEventDb::ItemStatus::Renamed;
    QStringList renamedFolderList = folderQueryResult.value(status);
    QStringList renamedFileList = fileQueryResult.value(status);

    QJsonObject jsonObject;
    jsonObject.insert("deletedFolderList", QJsonArray::fromStringList(deletedFolderList));
    jsonObject.insert("renamedFolderList", QJsonArray::fromStringList(renamedFolderList));
    jsonObject.insert("updatedFileList", QJsonArray::fromStringList(updatedFileList));
    jsonObject.insert("deletedFileList", QJsonArray::fromStringList(deletedFileList));
    jsonObject.insert("renamedFileList", QJsonArray::fromStringList(renamedFileList));

    QHttpServerResponse response = QHttpServerResponse(jsonObject, QHttpServerResponse::StatusCode::Ok);
    response.setHeader("Access-Control-Allow-Origin", "*");

    return response;
}

QHttpServerResponse getNewAddedEvents(FileSystemEventDb *fsEventDb)
{
    QStringList newAddedFolderList = fsEventDb->getNewAddedFolderMap().keys();
    QStringList newAddedFileList = fsEventDb->getNewAddedFileList();

    QJsonObject jsonObject;
    jsonObject.insert("newAddedFolderList", QJsonArray::fromStringList(newAddedFolderList));
    jsonObject.insert("newAddedFileList", QJsonArray::fromStringList(newAddedFileList));

    return QHttpServerResponse(jsonObject, QHttpServerResponse::StatusCode::Ok);
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

    QHttpServer httpServer;
    FileSystemEventDb *fsEventDb = new FileSystemEventDb();
    FileMonitoringManager fmm(fsEventDb);

    httpServer.route("/addNewFolder", QHttpServerRequest::Method::Post, [](const QHttpServerRequest &request) {
        return postAddNewFolder(request);
    });

    httpServer.route("/addNewFile", QHttpServerRequest::Method::Post, [](const QHttpServerRequest &request) {
        return postAddNewFile(request);
    });

    httpServer.route("/appendVersion", QHttpServerRequest::Method::Post, [](const QHttpServerRequest &request) {
        return postAppendVersion(request);
    });

    httpServer.route("/startMonitoring", QHttpServerRequest::Method::Post, [&fmm](const QHttpServerRequest &request) {
        return postStartMonitoring(fmm, request);
    });

    httpServer.route("/setStatusOfMonitoredFile", QHttpServerRequest::Method::Patch, [fsEventDb](const QHttpServerRequest &request) {
        return patchSetStatusOfMonitoredFile(fsEventDb, request);
    });

    httpServer.route("/getEventsOnFolderTree", QHttpServerRequest::Method::Get, [fsEventDb]() {
        return getEventsOnFolderTree(fsEventDb);
    });

    httpServer.route("/getNewAddedEvents", QHttpServerRequest::Method::Get, [fsEventDb]() {
        return getNewAddedEvents(fsEventDb);
    });

    quint16 targetPort = 1234; // Making this 0 means random port.
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
