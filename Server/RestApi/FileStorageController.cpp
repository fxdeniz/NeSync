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

QHttpServerResponse FileStorageController::addNewFolder(const QHttpServerRequest& request)
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

QHttpServerResponse FileStorageController::addNewFile(const QHttpServerRequest &request)
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

QHttpServerResponse FileStorageController::appendVersion(const QHttpServerRequest &request)
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
    QByteArray requestBody = request.body();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(requestBody);
    QJsonObject jsonObject = jsonDoc.object();

    QString symbolFolderPath = jsonObject["symbolPath"].toString();
    qDebug() << "symbolFolderPath = " << symbolFolderPath;

    auto fsm = FileStorageManager::instance();
    bool result = fsm->deleteFolder(symbolFolderPath);

    QJsonObject responseBody {{"isDeleted", result}};

    QHttpServerResponse response(responseBody);
    return response;
}

QHttpServerResponse FileStorageController::deleteFile(const QHttpServerRequest &request)
{
    QByteArray requestBody = request.body();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(requestBody);
    QJsonObject jsonObject = jsonDoc.object();

    QString symbolFilePath = jsonObject["symbolPath"].toString();
    qDebug() << "symbolFilePath = " << symbolFilePath;

    auto fsm = FileStorageManager::instance();
    bool result = fsm->deleteFile(symbolFilePath);

    QJsonObject responseBody {{"isDeleted", result}};

    QHttpServerResponse response(responseBody);
    return response;
}

QHttpServerResponse FileStorageController::getFolder(const QHttpServerRequest &request)
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

QHttpServerResponse FileStorageController::getFolderUserPath(const QHttpServerRequest &request)
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

QHttpServerResponse FileStorageController::getStorageFolderPath(const QHttpServerRequest &request)
{
    QJsonObject result;
    auto fsm = FileStorageManager::instance();

    qDebug() << "storageFolderPath = " << fsm->getStorageFolderPath();
    qDebug() << "";

    result.insert("storageFolderPath", fsm->getStorageFolderPath());

    return result;
}

QHttpServerResponse FileStorageController::getFile(const QHttpServerRequest &request)
{
    QByteArray requestBody = request.body();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(requestBody);
    QJsonObject jsonObject = jsonDoc.object();

    QString symbolFilePath = jsonObject["symbolPath"].toString();
    qDebug() << "symbolFilePath = " << symbolFilePath;
    qDebug() << "";

    auto fsm = FileStorageManager::instance();
    QJsonObject responseBody = fsm->getFileJsonBySymbolPath(symbolFilePath, true);

    QHttpServerResponse response(responseBody);
    return response;
}

QHttpServerResponse FileStorageController::getFileByUserPath(const QHttpServerRequest &request)
{
    QByteArray requestBody = request.body();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(requestBody);
    QJsonObject jsonObject = jsonDoc.object();

    QString userFilePath = jsonObject["userFilePath"].toString();
    qDebug() << "userFilePath = " << userFilePath;
    qDebug() << "";

    auto fsm = FileStorageManager::instance();
    QJsonObject responseBody = fsm->getFileJsonByUserPath(userFilePath, true);

    QHttpServerResponse response(responseBody);
    return response;
}

// TODO: improve input checking of this function.
QHttpServerResponse FileStorageController::updateFileFrozenStatus(const QHttpServerRequest &request)
{
    QByteArray requestBody = request.body();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(requestBody);
    QJsonObject jsonObject = jsonDoc.object();

    QString symbolFilePath = jsonObject["symbolPath"].toString();
    bool isFrozen = jsonObject["isFrozen"].toBool();
    qDebug() << "symbolFilePath = " << symbolFilePath;
    qDebug() << "isFrozen = " << isFrozen;
    qDebug() << "";

    auto fsm = FileStorageManager::instance();
    QJsonObject entity = fsm->getFileJsonBySymbolPath(symbolFilePath);

    entity[JsonKeys::File::IsFrozen] = isFrozen;

    bool isUpdated = fsm->updateFileEntity(entity);

    entity = fsm->getFileJsonBySymbolPath(symbolFilePath);

    entity["isUpdated"] = isUpdated;

    QHttpServerResponse response(entity);
    return response;
}

// TODO: Fix versionNumber boundries when converting from long long to unsigned long long.
//       Also, check negative inputs.
QHttpServerResponse FileStorageController::updateFileVersionDescription(const QHttpServerRequest &request)
{
    QByteArray requestBody = request.body();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(requestBody);
    QJsonObject jsonObject = jsonDoc.object();

    QString symbolFilePath = jsonObject["symbolFilePath"].toString();
    qulonglong versionNumber = jsonObject["versionNumber"].toInteger();
    QString description = jsonObject["description"].toString();
    qDebug() << "symbolFilePath = " << symbolFilePath;
    qDebug() << "versionNumber = " << versionNumber;
    qDebug() << "description = " << description;
    qDebug() << "";

    auto fsm = FileStorageManager::instance();
    QJsonObject entity = fsm->getFileVersionJson(symbolFilePath, versionNumber);

    entity[JsonKeys::FileVersion::Description] = description;

    bool isUpdated = fsm->updateFileVersionEntity(entity);

    entity = fsm->getFileVersionJson(symbolFilePath, versionNumber);

    entity["isUpdated"] = isUpdated;

    QHttpServerResponse response(entity);
    return response;
}
