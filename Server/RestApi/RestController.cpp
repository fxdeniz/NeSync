#include "RestController.h"

#include "JsonDtoFormat.h"
#include "FileStorageSubSystem/FileStorageManager.h"

#include <QJsonObject>
#include <QDirIterator>
#include <QJsonDocument>

RestController::RestController(QObject *parent)
    : QObject{parent}
{
}

QHttpServerResponse RestController::postAddNewFolder(const QHttpServerRequest& request)
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
    return response;
}

QHttpServerResponse RestController::newAddedList(const QHttpServerRequest &request)
{
    QJsonObject responseBody;

    auto fsm = FileStorageManager::instance();

    QStringList newFolderList;
    QMultiHash<QString, QString> newFileMap;
    QSet<QString> visitedSet;

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

                if(!isFolderPersists && !visitedSet.contains(path))
                {
                    newFolderList.append(path);
                    visitedSet.insert(path);
                }
            }
            else if(info.isFile())
            {
                QJsonObject fileJson = fsm->getFileJsonByUserPath(path);
                bool isFilePersists = fileJson[JsonKeys::IsExist].toBool();

                if(!isFilePersists && !visitedSet.contains(path))
                {
                    QString parentPath = QDir::toNativeSeparators(info.absolutePath());

                    if(!parentPath.endsWith(QDir::separator()))
                        parentPath.append(QDir::separator());

                    newFileMap.insert(parentPath, info.fileName());
                    visitedSet.insert(path);
                }
            }
        }
    }

    std::sort(newFolderList.begin(), newFolderList.end(), [](const QString &s1, const QString &s2) {
        return s1.length() < s2.length();
    });

    responseBody.insert("folders", QJsonArray::fromStringList(newFolderList));

    QJsonObject newFilesObject;

    for(const QString &parentPath : newFileMap.keys())
    {
        QStringList files = newFileMap.values(parentPath);
        newFilesObject.insert(parentPath, QJsonArray::fromStringList(files));
    }

    responseBody.insert("files", newFilesObject);

    return responseBody;
}

QHttpServerResponse RestController::deletedList(const QHttpServerRequest &request)
{
    QJsonObject responseBody;

    auto fsm = FileStorageManager::instance();

    QStringList folderList;
    QMultiHash<QString, QString> fileMap;

    for(const QJsonValue &folderObject : fsm->getActiveFolderList())
    {
        QString folderPath = folderObject[JsonKeys::Folder::UserFolderPath].toString();
        QFileInfo folderInfo(folderPath);
        bool isFolderFrozen = folderObject[JsonKeys::Folder::IsFrozen].toBool();

        if(!folderInfo.exists() && !isFolderFrozen)
            folderList.append(folderPath);

        QJsonObject fatFolderJson = fsm->getFolderJsonByUserPath(folderPath, true);

        for(const QJsonValue &fileObject : fatFolderJson[JsonKeys::Folder::ChildFiles].toArray())
        {
            QString filePath = fileObject[JsonKeys::File::UserFilePath].toString();
            QString fileName = fileObject[JsonKeys::File::FileName].toString();
            QFileInfo fileInfo(filePath);
            bool isFileFrozen = fileObject[JsonKeys::File::IsFrozen].toBool();

            if(!fileInfo.exists() && !isFileFrozen)
                fileMap.insert(folderPath, fileName);
        }
    }

    std::sort(folderList.begin(), folderList.end(), [](const QString &s1, const QString &s2) {
        return s1.length() < s2.length();
    });

    responseBody.insert("folders", QJsonArray::fromStringList(folderList));

    QJsonObject newFilesObject;

    for(const QString &parentPath : fileMap.keys())
    {
        QStringList files = fileMap.values(parentPath);
        newFilesObject.insert(parentPath, QJsonArray::fromStringList(files));
    }

    responseBody.insert("files", newFilesObject);

    return responseBody;
}

QHttpServerResponse RestController::updatedFileList(const QHttpServerRequest &request)
{
    QJsonObject responseBody;

    auto fsm = FileStorageManager::instance();

    QMultiHash<QString, QString> fileMap;

    for(const QJsonValue &value : fsm->getActiveFolderList())
    {
        QDirIterator dirIterator(value[JsonKeys::Folder::UserFolderPath].toString(),
                                 QDir::Filter::Files | QDir::Filter::NoDotAndDotDot);

        while (dirIterator.hasNext())
        {
            QString path = QDir::toNativeSeparators(dirIterator.next());

            QJsonObject fileJson = fsm->getFileJsonByUserPath(path);

            bool isFilePersists = fileJson[JsonKeys::IsExist].toBool();
            bool isFileFrozen = fileJson[JsonKeys::File::IsFrozen].toBool();

            if(isFilePersists && !isFileFrozen)
            {
                QString symbolPath = fileJson[JsonKeys::File::SymbolFilePath].toString();

                qlonglong versionNumber = fileJson[JsonKeys::File::MaxVersionNumber].toInteger();
                QJsonObject versionJson = fsm->getFileVersionJson(symbolPath, versionNumber);

                QDateTime savedTimestamp = QDateTime::fromString(versionJson[JsonKeys::FileVersion::LastModifiedTimestamp].toString(),
                                                                 Qt::DateFormat::ISODateWithMs);

                QFileInfo info(path);
                QString parentPath = QDir::toNativeSeparators(info.absolutePath());

                QDateTime lastTimestamp = info.lastModified();

                if(!parentPath.endsWith(QDir::separator()))
                    parentPath.append(QDir::separator());

                if(lastTimestamp != savedTimestamp)
                    fileMap.insert(parentPath, info.fileName());
            }
        }
    }

    //TODO: add sorting by parentPath
    for(const QString &parentPath : fileMap.keys())
    {
        QStringList files = fileMap.values(parentPath);
        responseBody.insert(parentPath, QJsonArray::fromStringList(files));
    }

    return responseBody;
}
