#include "ZipExportController.h"


#include <QJsonObject>
#include <QJsonDocument>
#include <QOperatingSystemVersion>


ZipExportController::ZipExportController(QObject *parent)
    : QObject{parent}
{}

QHttpServerResponse ZipExportController::setFilePath(const QHttpServerRequest &request)
{
    QByteArray requestBody = request.body();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(requestBody);
    QJsonObject jsonObject = jsonDoc.object();

    QString filePath = jsonObject["filePath"].toString();

    if(QOperatingSystemVersion::currentType() == QOperatingSystemVersion::OSType::MacOS)
        filePath = filePath.normalized(QString::NormalizationForm::NormalizationForm_D);

    qDebug() << "filePath = " << filePath;
    qDebug() << "";

    service.setZipFilePath(filePath);

    return QHttpServerResponse(QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse ZipExportController::getFilePath(const QHttpServerRequest &request)
{
    QJsonObject responseBody {{"filePath", service.getZipFilePath()}};
    QHttpServerResponse response = QHttpServerResponse(responseBody, QHttpServerResponse::StatusCode::Ok);

    return response;
}

QHttpServerResponse ZipExportController::setRootFolder(const QHttpServerRequest &request)
{
    QByteArray requestBody = request.body();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(requestBody);
    QJsonObject jsonObject = jsonDoc.object();

    QString rootSymbolFolderPath = jsonObject["rootSymbolFolderPath"].toString();

    // TODO: Make a research whether normalization here is necessary or not.
    //if(QOperatingSystemVersion::currentType() == QOperatingSystemVersion::OSType::MacOS)
    //    rootSymbolFolderPath = rootSymbolFolderPath.normalized(QString::NormalizationForm::NormalizationForm_D);

    qDebug() << "rootSymbolFolderPath = " << rootSymbolFolderPath;
    qDebug() << "";

    service.setRootSymbolFolderPath(rootSymbolFolderPath);

    return QHttpServerResponse(QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse ZipExportController::getRootFolder(const QHttpServerRequest &request)
{
    QJsonObject responseBody {{"rootPath", service.getRootSymbolFolderPath()}};
    QHttpServerResponse response = QHttpServerResponse(responseBody, QHttpServerResponse::StatusCode::Ok);

    return response;
}

QHttpServerResponse ZipExportController::createZip(const QHttpServerRequest &request)
{
    QJsonObject responseBody {{"isCreated", service.createArchive()}};
    QHttpServerResponse response = QHttpServerResponse(responseBody, QHttpServerResponse::StatusCode::Ok);

    return response;
}

QHttpServerResponse ZipExportController::addFoldersJson(const QHttpServerRequest &request)
{
    QJsonObject responseBody {{"isAdded", service.addFoldersJson()}};

    return QHttpServerResponse(responseBody, QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse ZipExportController::addFilesJson(const QHttpServerRequest &request)
{
    QJsonObject responseBody {{"isAdded", service.addFileJson()}};
    responseBody.insert("files", service.getFilesJson());

    return QHttpServerResponse(responseBody, QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse ZipExportController::addFile(const QHttpServerRequest &request)
{
    QByteArray requestBody = request.body();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(requestBody);
    QJsonObject jsonObject = jsonDoc.object();

    QString symbolFilePath = jsonObject["symbolFilePath"].toString();
    qlonglong versionNumber = jsonObject["versionNumber"].toInteger();

    qDebug() << "symbolFilePath = " << symbolFilePath;
    qDebug() << "versionNumber = " << versionNumber;
    qDebug() << "";

    QJsonObject responseBody {{"isAdded", service.addFileToZip(symbolFilePath, versionNumber)}};
    return QHttpServerResponse(responseBody, QHttpServerResponse::StatusCode::Ok);
}
