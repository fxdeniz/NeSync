#include "RestController.h"

#include "JsonDtoFormat.h"
#include "FileStorageSubSystem/FileStorageManager.h"
#include "FileMonitorSubSystem/FileMonitoringManager.h"

#include <QJsonObject>
#include <QDirIterator>
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

    auto fsm = FileStorageManager::instance();

    QJsonObject deletedFoldersObject;

    for(const QString &folderPath : fses->folderList())
    {
        FileSystemEventStore::Status status = fses->statusOfFolder(folderPath);

        if(status == FileSystemEventStore::Status::Deleted)
        {
            QJsonObject folderJson = fsm->getFolderJsonByUserPath(folderPath);

            folderJson.remove("isExist");
            folderJson.remove("isFrozen");
            folderJson.remove("childFolders");
            folderJson.remove("childFiles");
            deletedFoldersObject.insert(folderPath, folderJson);
        }
    }

    responseBody.insert("deletedFolders", deletedFoldersObject);

    QJsonObject updatedFilesObject, deletedFilesObject;

    for(const QString &filePath : fses->fileList())
    {
        FileSystemEventStore::Status status = fses->statusOfFile(filePath);
        QJsonObject fileJson = fsm->getFileJsonByUserPath(filePath);

        fileJson.remove("isExist");
        fileJson.remove("isFrozen");
        fileJson.remove("maxVersionNumber");
        fileJson.remove("versionList");
        QJsonObject folderJson = fsm->getFolderJsonBySymbolPath(fileJson[JsonKeys::File::SymbolFolderPath].toString());

        QString parentFolderPath = folderJson[JsonKeys::Folder::UserFolderPath].toString();

        if(status == FileSystemEventStore::Status::Updated)
        {
            QJsonArray parentFolderArray = updatedFilesObject[parentFolderPath].toArray();
            parentFolderArray.append(fileJson);

            updatedFilesObject.insert(parentFolderPath, parentFolderArray);
        }
        else if(status == FileSystemEventStore::Status::Deleted)
        {
            QJsonArray parentFolderArray = deletedFilesObject[parentFolderPath].toArray();
            parentFolderArray.append(fileJson);

            deletedFilesObject.insert(parentFolderPath, parentFolderArray);
        }
    }

    responseBody.insert("updatedFiles", updatedFilesObject);
    responseBody.insert("deletedFiles", deletedFilesObject);

    QHttpServerResponse response(responseBody, QHttpServerResponse::StatusCode::Ok);
    response.addHeader("Access-Control-Allow-Origin", "*");
    return response;
}

QHttpServerResponse RestController::newAddedList(const QHttpServerRequest &request)
{
    QJsonObject responseBody;

    auto fsm = FileStorageManager::instance();

    QStringList newFolderList;
    QMultiHash<QString, QString> newFileMap;

    for(const QJsonValue &value : fsm->getActiveFolderList())
    {
        QDirIterator dirIterator(value[JsonKeys::Folder::UserFolderPath].toString(),
                                 QDir::Filter::Files | QDir::Filter::Dirs | QDir::Filter::NoDotAndDotDot,
                                 QDirIterator::IteratorFlag::Subdirectories);

        while (dirIterator.hasNext())
        {
            QString path = QDir::toNativeSeparators(dirIterator.next());
            QFileInfo info = dirIterator.fileInfo();

            if(info.isDir())
            {
                if(!path.endsWith(QDir::separator()))
                    path.append(QDir::separator());

                QJsonObject folderJson = fsm->getFolderJsonByUserPath(path);
                bool isFolderPersists = folderJson[JsonKeys::IsExist].toBool();

                if(!isFolderPersists)
                    newFolderList.append(path);
            }
            else if(info.isFile())
            {
                QJsonObject fileJson = fsm->getFileJsonByUserPath(path);
                bool isFilePersists = fileJson[JsonKeys::IsExist].toBool();

                if(!isFilePersists)
                {
                    QFileInfo info(path);
                    QString parentPath = QDir::toNativeSeparators(info.absolutePath());

                    if(!parentPath.endsWith(QDir::separator()))
                        parentPath.append(QDir::separator());

                    newFileMap.insert(parentPath, path);
                }
            }
        }
    }

    std::sort(newFolderList.begin(), newFolderList.end(), [](const QString &s1, const QString &s2) {
        return s1.length() < s2.length();
    });

    responseBody.insert("newFolders", QJsonArray::fromStringList(newFolderList));

    QJsonObject newFilesObject;

    for(const QString &parentPath : newFileMap.keys())
    {
        QStringList files = newFileMap.values(parentPath);
        newFilesObject.insert(parentPath, QJsonArray::fromStringList(files));
    }

    responseBody.insert("newFiles", newFilesObject);

    return responseBody;
}
