#include "ZipImportController.h"

#include <QJsonDocument>
#include <QHttpServerRequest>
#include <QOperatingSystemVersion>


ZipImportController::ZipImportController(QObject *parent)
    : QObject{parent}
{}

QHttpServerResponse ZipImportController::setFilePath(const QHttpServerRequest &request)
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

QHttpServerResponse ZipImportController::getFilePath(const QHttpServerRequest &request)
{
    QJsonObject responseBody {{"filePath", service.getZipFilePath()}};
    QHttpServerResponse response = QHttpServerResponse(responseBody, QHttpServerResponse::StatusCode::Ok);

    return response;
}

QHttpServerResponse ZipImportController::openZip(const QHttpServerRequest &request)
{
    QJsonObject responseBody {{"isOpened", service.openArchive()}};
    QHttpServerResponse response = QHttpServerResponse(responseBody, QHttpServerResponse::StatusCode::Ok);

    return response;
}

QHttpServerResponse ZipImportController::readFoldersJson(const QHttpServerRequest &request)
{
    QJsonArray responseBody {service.readFoldersJson()};

    return QHttpServerResponse(responseBody, QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse ZipImportController::readFilesJson(const QHttpServerRequest &request)
{
    QJsonObject responseBody {service.readFilesJson()};

    return QHttpServerResponse(responseBody, QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse ZipImportController::importFileFromZip(const QHttpServerRequest &request)
{
    QByteArray requestBody = request.body();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(requestBody);
    QJsonObject jsonObject = jsonDoc.object();

    QString symbolFilePath = jsonObject["symbolFilePath"].toString();
    qulonglong versionNumber = jsonObject["versionNumber"].toInteger();

    qDebug() << "symbolFilePath = " << symbolFilePath;
    qDebug() << "versionNumber = " << versionNumber;
    qDebug() << "";

    QJsonObject responseBody {{"isImported", service.importFile(symbolFilePath, versionNumber)}};

    return QHttpServerResponse(responseBody, QHttpServerResponse::StatusCode::Ok);
}
