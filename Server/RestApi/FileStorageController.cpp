#include "FileStorageController.h"

#include "JsonDtoFormat.h"
#include "FileStorageSubSystem/FileStorageManager.h"

#include <QJsonObject>
#include <QDirIterator>
#include <QJsonDocument>
#include <QOperatingSystemVersion>

FileStorageController::FileStorageController(QObject *parent)
    : QObject{parent}
{
}

QHttpServerResponse FileStorageController::postAddNewFolder_V1(const QHttpServerRequest& request)
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
    QString userFolderPath = jsonObject["userFolderPath"].toString();

    if(QOperatingSystemVersion::currentType() == QOperatingSystemVersion::OSType::MacOS)
    {
        symbolFolderPath = symbolFolderPath.normalized(QString::NormalizationForm::NormalizationForm_D);
        userFolderPath = userFolderPath.normalized(QString::NormalizationForm::NormalizationForm_D);
    }

    qDebug() << "symbolFolderPath = " << symbolFolderPath;
    qDebug() << "userFolderPath = " << userFolderPath;

    auto fsm = FileStorageManager::instance();
    bool isAdded = fsm->addNewFolder(symbolFolderPath, userFolderPath);

    if(isAdded)
    {
        QString reponseMessage = "Folder is created.";
        QHttpServerResponse response = QHttpServerResponse(reponseMessage, QHttpServerResponse::StatusCode::Created);

        return response;
    }

    return response;
}

// Version 2 more straight forward.
QHttpServerResponse FileStorageController::postAddNewFolder(const QHttpServerRequest& request)
{
    QByteArray requestBody = request.body();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(requestBody);
    QJsonObject jsonObject = jsonDoc.object();

    QString symbolFolderPath = jsonObject["symbolFolderPath"].toString();
    QString userFolderPath = jsonObject["userFolderPath"].toString();

    if(QOperatingSystemVersion::currentType() == QOperatingSystemVersion::OSType::MacOS)
    {
        symbolFolderPath = symbolFolderPath.normalized(QString::NormalizationForm::NormalizationForm_D);
        userFolderPath = userFolderPath.normalized(QString::NormalizationForm::NormalizationForm_D);
    }

    qDebug() << "symbolFolderPath = " << symbolFolderPath;
    qDebug() << "userFolderPath = " << userFolderPath;

    auto fsm = FileStorageManager::instance();
    bool isAdded = fsm->addNewFolder(symbolFolderPath, userFolderPath);

    QJsonObject responseBody {{"isAdded", isAdded}};
    QHttpServerResponse response = QHttpServerResponse(responseBody, QHttpServerResponse::StatusCode::Ok);

    return response;
}

QHttpServerResponse FileStorageController::postAddNewFile_V1(const QHttpServerRequest &request)
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

    if(QOperatingSystemVersion::currentType() == QOperatingSystemVersion::OSType::MacOS)
    {
        symbolFolderPath = symbolFolderPath.normalized(QString::NormalizationForm::NormalizationForm_D);
        pathToFile = pathToFile.normalized(QString::NormalizationForm::NormalizationForm_D);
    }

    qDebug() << "symbolFolderPath = " << symbolFolderPath;
    qDebug() << "pathToFile = " << pathToFile;
    qDebug() << "description = " << description;
    qDebug() << "isFrozen = " << isFrozen;

    auto fsm = FileStorageManager::instance();

    QJsonObject folderJson = fsm->getFolderJsonBySymbolPath(symbolFolderPath);

    if(folderJson[JsonKeys::IsExist].toBool())
    {
        bool isAdded = fsm->addNewFile(symbolFolderPath, pathToFile, isFrozen, "", description);

        qDebug() << "isAdded = " << isAdded;
        qDebug() << "";

        if(isAdded)
        {
            QString reponseMessage = "File is created.";
            response = QHttpServerResponse(reponseMessage, QHttpServerResponse::StatusCode::Created);
            return response;
        }
    }

    return response;
}

// Version 2 more straight forward.
QHttpServerResponse FileStorageController::postAddNewFile(const QHttpServerRequest &request)
{
    QByteArray requestBody = request.body();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(requestBody);
    QJsonObject jsonObject = jsonDoc.object();

    QString symbolFolderPath = jsonObject["symbolFolderPath"].toString();
    QString pathToFile = jsonObject["pathToFile"].toString();
    QString description = jsonObject["description"].toString();
    bool isFrozen = jsonObject["isFrozen"].toBool();

    if(QOperatingSystemVersion::currentType() == QOperatingSystemVersion::OSType::MacOS)
    {
        symbolFolderPath = symbolFolderPath.normalized(QString::NormalizationForm::NormalizationForm_D);
        pathToFile = pathToFile.normalized(QString::NormalizationForm::NormalizationForm_D);
    }

    qDebug() << "symbolFolderPath = " << symbolFolderPath;
    qDebug() << "pathToFile = " << pathToFile;
    qDebug() << "description = " << description;
    qDebug() << "isFrozen = " << isFrozen;

    auto fsm = FileStorageManager::instance();

    QJsonObject folderJson = fsm->getFolderJsonBySymbolPath(symbolFolderPath);

    bool isAdded = fsm->addNewFile(symbolFolderPath, pathToFile, isFrozen, "", description);

    qDebug() << "isAdded = " << isAdded;
    qDebug() << "";

    QJsonObject responseBody {{"isAdded", isAdded}};
    QHttpServerResponse response(responseBody, QHttpServerResponse::StatusCode::Ok);

    return response;
}

QHttpServerResponse FileStorageController::postAppendVersion_V1(const QHttpServerRequest &request)
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

    if(QOperatingSystemVersion::currentType() == QOperatingSystemVersion::OSType::MacOS)
        pathToFile = pathToFile.normalized(QString::NormalizationForm::NormalizationForm_D);

    qDebug() << "pathToFile = " << pathToFile;
    qDebug() << "description = " << description;

    auto fsm = FileStorageManager::instance();

    QJsonObject fileJson = fsm->getFileJsonByUserPath(pathToFile);

    if(fileJson[JsonKeys::IsExist].toBool())
    {
        bool isAppended = fsm->appendVersion(fileJson[JsonKeys::File::SymbolFilePath].toString(), pathToFile, description);

        qDebug() << "isAppended = " << isAppended;
        qDebug() << "";

        if(isAppended)
        {
            QString reponseMessage = "Version appended.";
            response = QHttpServerResponse(reponseMessage, QHttpServerResponse::StatusCode::Ok);
            return response;
        }
    }

    return response;
}

// Version 2 more straight forward.
QHttpServerResponse FileStorageController::postAppendVersion(const QHttpServerRequest &request)
{
    QByteArray requestBody = request.body();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(requestBody);
    QJsonObject jsonObject = jsonDoc.object();

    QString pathToFile = jsonObject["pathToFile"].toString();
    QString description = jsonObject["description"].toString();

    if(QOperatingSystemVersion::currentType() == QOperatingSystemVersion::OSType::MacOS)
        pathToFile = pathToFile.normalized(QString::NormalizationForm::NormalizationForm_D);

    qDebug() << "pathToFile = " << pathToFile;
    qDebug() << "description = " << description;

    auto fsm = FileStorageManager::instance();

    QJsonObject fileJson = fsm->getFileJsonByUserPath(pathToFile);

    bool isAppended = fsm->appendVersion(fileJson[JsonKeys::File::SymbolFilePath].toString(), pathToFile, description);

    qDebug() << "isAppended = " << isAppended;
    qDebug() << "";

    QJsonObject responseBody {{"isAppended", isAppended}};

    QHttpServerResponse response(responseBody, QHttpServerResponse::StatusCode::Ok);

    return response;
}

QHttpServerResponse FileStorageController::deleteFolder(const QHttpServerRequest &request)
{
    QString symbolFolderPath = request.query().queryItemValue("symbolPath");
    qDebug() << "symbolFolderPath = " << symbolFolderPath;

    auto fsm = FileStorageManager::instance();
    bool result = fsm->deleteFolder(symbolFolderPath);

    QJsonObject responseBody {{"isDeleted", result}};

    QHttpServerResponse response(responseBody);
    return response;
}

QHttpServerResponse FileStorageController::deleteFile(const QHttpServerRequest &request)
{
    QString symbolFilePath = request.query().queryItemValue("symbolPath");
    qDebug() << "symbolFilePath = " << symbolFilePath;

    auto fsm = FileStorageManager::instance();
    bool result = fsm->deleteFile(symbolFilePath);

    QJsonObject responseBody {{"isDeleted", result}};

    QHttpServerResponse response(responseBody);
    return response;
}

QHttpServerResponse FileStorageController::getFolderContent(const QHttpServerRequest &request)
{
    QByteArray requestBody = request.body();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(requestBody);
    QJsonObject jsonObject = jsonDoc.object();

    QString symbolFolderPath = jsonObject["symbolPath"].toString();
    qDebug() << "symbolFolderPath = " << symbolFolderPath;

    auto fsm = FileStorageManager::instance();
    QJsonObject responseBody = fsm->getFolderJsonBySymbolPath(symbolFolderPath, true);

    QHttpServerResponse response(responseBody);
    return response;
}

QHttpServerResponse FileStorageController::getFolderContentByUserPath(const QHttpServerRequest &request)
{
    QByteArray requestBody = request.body();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(requestBody);
    QJsonObject jsonObject = jsonDoc.object();

    QString userFolderPath = jsonObject["userFolderPath"].toString();
    qDebug() << "userFolderPath = " << userFolderPath;

    auto fsm = FileStorageManager::instance();
    QJsonObject responseBody = fsm->getFolderJsonByUserPath(userFolderPath, true);

    QHttpServerResponse response(responseBody);
    return response;
}

QHttpServerResponse FileStorageController::getFileContent(const QHttpServerRequest &request)
{
    QString symbolFilePath = request.query().queryItemValue("symbolPath", QUrl::ComponentFormattingOption::FullyDecoded);
    qDebug() << "symbolFilePath = " << symbolFilePath;
    qDebug() << "";

    auto fsm = FileStorageManager::instance();
    QJsonObject responseBody = fsm->getFileJsonBySymbolPath(symbolFilePath, true);

    QHttpServerResponse response(responseBody);
    return response;
}

QHttpServerResponse FileStorageController::getFileContentByUserPath(const QHttpServerRequest &request)
{
    QString userFilePath = request.query().queryItemValue("userFilePath");
    qDebug() << "userFilePath = " << userFilePath;
    qDebug() << "";

    auto fsm = FileStorageManager::instance();
    QJsonObject responseBody = fsm->getFileJsonByUserPath(userFilePath, true);

    QHttpServerResponse response(responseBody);
    return response;
}
