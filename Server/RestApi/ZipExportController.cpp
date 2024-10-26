#include "ZipExportController.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <quazip/quazip.h>


ZipExportController::ZipExportController(QObject *parent)
    : QObject{parent}
{}

QHttpServerResponse ZipExportController::postCreateArchive(const QHttpServerRequest &request)
{
    QByteArray requestBody = request.body();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(requestBody);
    QJsonObject jsonObject = jsonDoc.object();

    QString folderPath = jsonObject["folderPath"].toString();
    QString fileName = jsonObject["fileName"].toString();

    qDebug() << "folderPath = " << folderPath;
    qDebug() << "fileName = " << fileName;

    QString zipFilePath = folderPath + fileName;

    QuaZip archive(zipFilePath);
    bool isCreated = archive.open(QuaZip::Mode::mdCreate);

    QJsonObject responseBody {{"isCreated", isCreated}};
    QHttpServerResponse response = QHttpServerResponse(responseBody, QHttpServerResponse::StatusCode::Ok);

    return response;
}
