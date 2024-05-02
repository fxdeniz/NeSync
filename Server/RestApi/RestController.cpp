#include "RestController.h"

#include "JsonDtoFormat.h"
#include "FileStorageSubSystem/FileStorageManager.h"
#include "FileMonitorSubSystem/FileMonitoringManager.h"
#include <QJsonObject>
#include <QJsonDocument>

RestController::RestController(QObject *parent)
    : QObject{parent}
{
    fileMonitorThread = nullptr;
    fses = nullptr;
}

QHttpServerResponse RestController::postAddNewFolder(const QHttpServerRequest& request)
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

QHttpServerResponse RestController::postAddNewFile(const QHttpServerRequest &request)
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

QHttpServerResponse RestController::postAppendVersion(const QHttpServerRequest &request)
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

QHttpServerResponse RestController::getFolderContent(const QHttpServerRequest &request)
{
    QString symbolFolderPath = request.query().queryItemValue("symbolPath");
    qDebug() << "symbolFolderPath = " << symbolFolderPath;

    auto fsm = FileStorageManager::instance();
    QJsonObject responseBody = fsm->getFolderJsonBySymbolPath(symbolFolderPath, true);

    QHttpServerResponse response(responseBody);
    response.addHeader("Access-Control-Allow-Origin", "*");
    return response;
}

QHttpServerResponse RestController::startMonitoring(const QHttpServerRequest &request)
{
    if(fileMonitorThread != nullptr)
    {
        fileMonitorThread->quit();
        fileMonitorThread->wait();
        delete fileMonitorThread;
        fileMonitorThread = nullptr;
    }

    fileMonitorThread = new QThread();
    fses = new FileSystemEventStore();

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

QHttpServerResponse RestController::dumpFses(const QHttpServerRequest &request)
{
    QJsonObject responseBody;

    if(fses == nullptr)
    {
        QString errorMessage = "Fses is null";
        QHttpServerResponse response(errorMessage, QHttpServerResponse::StatusCode::NotImplemented);
        response.addHeader("Access-Control-Allow-Origin", "*");

        return response;
    }

    for(const QString &currentFolderPath : fses->folderList())
    {
        responseBody.insert(currentFolderPath, fses->statusOfFolder(currentFolderPath));
    }

    QHttpServerResponse response(responseBody, QHttpServerResponse::StatusCode::Ok);
    response.addHeader("Access-Control-Allow-Origin", "*");
    return response;
}
