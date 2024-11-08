#include "FileSystemMonitorController.h"

#include <QJsonArray>
#include <QJsonObject>

FileSystemMonitorController::FileSystemMonitorController(QObject *parent)
    : QObject{parent}
{}

QHttpServerResponse FileSystemMonitorController::newAddedItems(const QHttpServerRequest &request)
{
    QStringList rootFolderList = service.generateRootFoldersList();
    QJsonObject responseBody;

    responseBody.insert("rootFolders", QJsonArray::fromStringList(rootFolderList));
    responseBody.insert("childFolderSuffixes", service.generateChildFolderSuffixObject(rootFolderList));
    responseBody.insert("rootOfRootFolder", service.generateRootOfRootFoldersObject(rootFolderList));
    responseBody.insert("files", service.generateFilesObject(rootFolderList));
    responseBody.insert("folders", QJsonArray::fromStringList(service.generateFoldersList(rootFolderList)));

    QHttpServerResponse response(responseBody, QHttpServerResponse::StatusCode::Ok);
    return response;
}

QHttpServerResponse FileSystemMonitorController::deletedItems(const QHttpServerRequest &request)
{
    return service.deletedItemsObject();
}

QHttpServerResponse FileSystemMonitorController::updatedFiles(const QHttpServerRequest &request)
{
    return service.updatedFilesObject();
}
