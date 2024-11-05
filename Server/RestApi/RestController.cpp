#include "RestController.h"

#include "JsonDtoFormat.h"
#include "FileStorageSubSystem/FileStorageManager.h"

#include <QJsonObject>
#include <QDirIterator>
#include <QJsonDocument>
#include <QOperatingSystemVersion>

RestController::RestController(QObject *parent)
    : QObject{parent}
{
}

QHttpServerResponse RestController::postAddNewFolder_V1(const QHttpServerRequest& request)
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
QHttpServerResponse RestController::postAddNewFolder(const QHttpServerRequest& request)
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

QHttpServerResponse RestController::postAddNewFile_V1(const QHttpServerRequest &request)
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
QHttpServerResponse RestController::postAddNewFile(const QHttpServerRequest &request)
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

QHttpServerResponse RestController::postAppendVersion_V1(const QHttpServerRequest &request)
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
QHttpServerResponse RestController::postAppendVersion(const QHttpServerRequest &request)
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

QHttpServerResponse RestController::deleteFolder(const QHttpServerRequest &request)
{
    QString symbolFolderPath = request.query().queryItemValue("symbolPath");
    qDebug() << "symbolFolderPath = " << symbolFolderPath;

    auto fsm = FileStorageManager::instance();
    bool result = fsm->deleteFolder(symbolFolderPath);

    QJsonObject responseBody {{"isDeleted", result}};

    QHttpServerResponse response(responseBody);
    return response;
}

QHttpServerResponse RestController::deleteFile(const QHttpServerRequest &request)
{
    QString symbolFilePath = request.query().queryItemValue("symbolPath");
    qDebug() << "symbolFilePath = " << symbolFilePath;

    auto fsm = FileStorageManager::instance();
    bool result = fsm->deleteFile(symbolFilePath);

    QJsonObject responseBody {{"isDeleted", result}};

    QHttpServerResponse response(responseBody);
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

QHttpServerResponse RestController::getFolderContentByUserPath(const QHttpServerRequest &request)
{
    QString userFolderPath = request.query().queryItemValue("userFolderPath");
    qDebug() << "userFolderPath = " << userFolderPath;

    auto fsm = FileStorageManager::instance();
    QJsonObject responseBody = fsm->getFolderJsonByUserPath(userFolderPath, true);

    QHttpServerResponse response(responseBody);
    return response;
}

QHttpServerResponse RestController::getFileContentByUserPath(const QHttpServerRequest &request)
{
    QString userFilePath = request.query().queryItemValue("userFilePath");
    qDebug() << "userFilePath = " << userFilePath;

    auto fsm = FileStorageManager::instance();
    QJsonObject responseBody = fsm->getFileJsonByUserPath(userFilePath, true);

    QHttpServerResponse response(responseBody);
    return response;
}

QHttpServerResponse RestController::newAddedList_V1(const QHttpServerRequest &request)
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

QHttpServerResponse RestController::newAddedList_V2(const QHttpServerRequest &request)
{
    QJsonObject responseBody;

    auto fsm = FileStorageManager::instance();
    QSet<QString> existingFolderSet, existingFileSet;
    QStringList existingFolderList;

    for(const QJsonValue &value : fsm->getActiveFolderList())
    {
        QString symbolPath = value[JsonKeys::Folder::SymbolFolderPath].toString();
        QString userPath = value[JsonKeys::Folder::UserFolderPath].toString();

        existingFolderSet.insert(userPath);
        existingFolderList.append(userPath);

        QJsonObject folderJson = fsm->getFolderJsonBySymbolPath(symbolPath, true);

        for(const QJsonValue &file : folderJson[JsonKeys::Folder::ChildFiles].toArray())
        {
            QString userFilePath = file[JsonKeys::File::UserFilePath].toString();
            existingFileSet.insert(userFilePath);
        }
    }

    std::sort(existingFolderList.begin(), existingFolderList.end(), [](const QString &s1, const QString &s2) {
        return s1.length() < s2.length();
    });


    QStringList newFolderList;
    QMultiHash<QString, QString> newFileMap;

    for(const QString &value : existingFolderList)
    {
        QDirIterator dirIterator(value,
                                 QDir::Filter::Files | QDir::Filter::Dirs | QDir::Filter::NoDotAndDotDot,
                                 QDirIterator::IteratorFlag::Subdirectories);

        while (dirIterator.hasNext())
        {
            QString path = QDir::toNativeSeparators(dirIterator.next());

            // MacOS normalization
            //https://ss64.com/mac/syntax-filenames.html
            if(QOperatingSystemVersion::currentType() == QOperatingSystemVersion::OSType::MacOS)
                path = path.normalized(QString::NormalizationForm::NormalizationForm_D);

            QFileInfo info = dirIterator.fileInfo();

            if(info.isDir() && !path.endsWith(QDir::separator()))
                path.append(QDir::separator());

            if(!existingFolderSet.contains(path) && !existingFileSet.contains(path))
            {
                if(info.isDir())
                    newFolderList.append(path);
                else if(info.isFile())
                {
                    QString parentPath = QDir::toNativeSeparators(info.absolutePath());

                    if(!parentPath.endsWith(QDir::separator()))
                        parentPath.append(QDir::separator());

                    newFileMap.insert(parentPath, info.fileName());
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

// Version 3, only visits each folder once.
QHttpServerResponse RestController::newAddedList_V3(const QHttpServerRequest &request)
{
    QJsonObject responseBody;

    auto fsm = FileStorageManager::instance();
    QSet<QString> existingFolderSet, existingFileSet;
    QStringList existingFolderList;

    for(const QJsonValue &value : fsm->getActiveFolderList())
    {
        QString symbolPath = value[JsonKeys::Folder::SymbolFolderPath].toString();
        QString userPath = value[JsonKeys::Folder::UserFolderPath].toString();

        existingFolderSet.insert(userPath);
        existingFolderList.append(userPath);

        QJsonObject folderJson = fsm->getFolderJsonBySymbolPath(symbolPath, true);

        for(const QJsonValue &file : folderJson[JsonKeys::Folder::ChildFiles].toArray())
        {
            QString userFilePath = file[JsonKeys::File::UserFilePath].toString();
            existingFileSet.insert(userFilePath);
        }
    }

    std::sort(existingFolderList.begin(), existingFolderList.end(), [](const QString &s1, const QString &s2) {
        return s1.length() < s2.length();
    });


    QStringList newRootFolderList;
    QMultiHash<QString, QString> newFileMap;

    for(const QString &value : existingFolderList)
    {
        QDirIterator dirIterator(value, QDir::Filter::Files | QDir::Filter::Dirs | QDir::Filter::NoDotAndDotDot);

        // TODO: remove isDir() checks inside this loop.
        while (dirIterator.hasNext())
        {
            QString path = QDir::toNativeSeparators(dirIterator.next());

            // MacOS normalization
            //https://ss64.com/mac/syntax-filenames.html
            if(QOperatingSystemVersion::currentType() == QOperatingSystemVersion::OSType::MacOS)
                path = path.normalized(QString::NormalizationForm::NormalizationForm_D);

            QFileInfo info = dirIterator.fileInfo();

            if(info.isDir() && !path.endsWith(QDir::separator()))
                path.append(QDir::separator());

            if(info.isDir() && !existingFolderSet.contains(path))
                newRootFolderList.append(path);
            else if(info.isFile() && !existingFileSet.contains(path))
            {
                QString parentPath = QDir::toNativeSeparators(info.absolutePath());

                if(!parentPath.endsWith(QDir::separator()))
                    parentPath.append(QDir::separator());

                if(QOperatingSystemVersion::currentType() == QOperatingSystemVersion::OSType::MacOS)
                    parentPath = parentPath.normalized(QString::NormalizationForm::NormalizationForm_D);

                newFileMap.insert(parentPath, info.fileName());
            }
        }
    }

    // TODO: remove this sorting if possible.
    std::sort(newRootFolderList.begin(), newRootFolderList.end(), [](const QString &s1, const QString &s2) {
        return s1.length() < s2.length();
    });

    QStringList newFolderList;
    QSet<QString> visitedFolderSet;

    for(const QString &value : newRootFolderList)
    {
        QDirIterator dirIterator(value,
                                 QDir::Filter::Files | QDir::Filter::Dirs | QDir::Filter::NoDotAndDotDot,
                                 QDirIterator::IteratorFlag::Subdirectories);

        while (dirIterator.hasNext())
        {
            QString path = QDir::toNativeSeparators(dirIterator.next());

            // MacOS normalization
            //https://ss64.com/mac/syntax-filenames.html
            if(QOperatingSystemVersion::currentType() == QOperatingSystemVersion::OSType::MacOS)
                path = path.normalized(QString::NormalizationForm::NormalizationForm_D);

            QFileInfo info = dirIterator.fileInfo();

            if(info.isDir() && !path.endsWith(QDir::separator()))
                path.append(QDir::separator());

            if(!visitedFolderSet.contains(path) && !existingFolderSet.contains(path) && !existingFileSet.contains(path))
            {
                if(info.isDir())
                {
                    newFolderList.append(path);
                    visitedFolderSet.insert(path);
                }
                else if(info.isFile())
                {
                    QString parentPath = QDir::toNativeSeparators(info.absolutePath());

                    if(!parentPath.endsWith(QDir::separator()))
                        parentPath.append(QDir::separator());

                    if(QOperatingSystemVersion::currentType() == QOperatingSystemVersion::OSType::MacOS)
                        parentPath = parentPath.normalized(QString::NormalizationForm::NormalizationForm_D);

                    newFileMap.insert(parentPath, info.fileName());
                }
            }
        }
    }

    newFolderList.append(newRootFolderList); // Append roots missed in the previous for loop.

    // TODO: Remove this sorting, because data comes already sorted from previous loop.
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

// Version 4, returns new added folder tree structure.
QHttpServerResponse RestController::newAddedList(const QHttpServerRequest &request)
{
    QJsonObject responseBody;

    QSet<QString> existingFolderSet, existingFileSet;
    QStringList existingFolderList;

    newAddedList_findExisting(existingFolderSet,
                              existingFileSet,
                              existingFolderList);

    QStringList newRootFolderList;
    QMultiHash<QString, QString> newFileMap;
    QHash<QString, QString> newRootFolderRootMap;

    newAddedList_findNewAtRootLevel(existingFolderSet,
                                    existingFileSet,
                                    existingFolderList,
                                    newRootFolderList,
                                    newFileMap,
                                    newRootFolderRootMap);

    QStringList newFolderList;
    QMultiHash<QString, QString> childFoldersOfNewRootFolderMap;

    newAddedList_findChildrenOfRootFolders(existingFolderSet,
                                           existingFileSet,
                                           newRootFolderList,
                                           newFolderList,
                                           childFoldersOfNewRootFolderMap,
                                           newFileMap);

    responseBody.insert("rootFolders", QJsonArray::fromStringList(newRootFolderList)); // Already sorted in newAddedList_findNewAtRootLevel().

    QJsonObject rootOfNewRootFolderObject, childFoldersOfNewRootFolderObject;

    for (auto it = newRootFolderRootMap.constBegin(); it != newRootFolderRootMap.constEnd(); ++it)
        rootOfNewRootFolderObject.insert(it.key(), it.value());

    responseBody.insert("rootOfRootFolder", rootOfNewRootFolderObject);

    for(const QString &parentPath : childFoldersOfNewRootFolderMap.uniqueKeys())
    {
        QStringList folders = childFoldersOfNewRootFolderMap.values(parentPath);

        for(QString &value : folders)
            value = value.split(parentPath).last();

        std::sort(folders.begin(), folders.end(), [](const QString &s1, const QString &s2) {
            return s1.length() < s2.length();
        });

        childFoldersOfNewRootFolderObject.insert(parentPath, QJsonArray::fromStringList(folders));
    }

    responseBody.insert("childFolderSuffixes", childFoldersOfNewRootFolderObject);

    newFolderList.append(newRootFolderList); // Append roots missed in the newAddedList_findChildrenOfRootFolders().

    std::sort(newFolderList.begin(), newFolderList.end(), [](const QString &s1, const QString &s2) {
        return s1.length() < s2.length();
    });

    responseBody.insert("folders", QJsonArray::fromStringList(newFolderList));

    QJsonObject newFilesObject;

    for(const QString &parentPath : newFileMap.uniqueKeys())
    {
        QStringList files = newFileMap.values(parentPath);
        newFilesObject.insert(parentPath, QJsonArray::fromStringList(files));
    }

    responseBody.insert("files", newFilesObject);

    return responseBody;
}

// Version 2, code is simpler.
QHttpServerResponse RestController::simpleNewAddedList(const QHttpServerRequest &request)
{
    QStringList rootFolderList = generateRootFoldersList();
    QStringList folderList = generateFoldersList();
    QJsonObject responseBody;

    responseBody.insert("rootFolders", QJsonArray::fromStringList(rootFolderList));
    responseBody.insert("folders", QJsonArray::fromStringList(folderList));
    responseBody.insert("childFolderSuffixes", generateChildFolderSuffixObject(rootFolderList));
    responseBody.insert("rootOfRootFolder", generateRootOfRootFoldersObject(rootFolderList));
    responseBody.insert("files", generateFilesObject());

    QHttpServerResponse response(responseBody, QHttpServerResponse::StatusCode::Ok);
    return response;
}

QHttpServerResponse RestController::simpleNewAddedList_V1(const QHttpServerRequest &request)
{
    auto fsm = FileStorageManager::instance();
    QStringList rootFolders;

    QJsonObject filesObj;

    for(const QJsonValue &value : fsm->getActiveFolderList())
    {
        QString path = value.toObject()[JsonKeys::Folder::UserFolderPath].toString();
        QStringList childFolders = findNewFolders(path);
        QStringList childFiles = findNewFiles(path); // Find files in existing folders non-recursively.

        if(!childFolders.isEmpty())
            rootFolders.append(childFolders);

        if(!childFiles.isEmpty())
        {
            for (qlonglong index = 0; index < childFiles.size(); ++index)
                childFiles[index] = childFiles[index].split(path).last();

            filesObj.insert(path, QJsonArray::fromStringList(childFiles));
        }
    }

    QStringList folders;
    QMultiHash<QString, QString> childFolderSuffixesOfRoot;

    for(const QString &rootPath : rootFolders)
    {
        QStringList childFolders = findNewFolders(rootPath, true);
        QStringList childFilesOfRoot = findNewFiles(rootPath); // Find files in new root folders non-recursively.
        folders.append(childFolders);

        if(!childFilesOfRoot.isEmpty())
        {
            for (qlonglong index = 0; index < childFilesOfRoot.size(); ++index)
                childFilesOfRoot[index] = childFilesOfRoot[index].split(rootPath).last();

            filesObj.insert(rootPath, QJsonArray::fromStringList(childFilesOfRoot));
        }

        for(const QString &child : childFolders)
        {
            QString suffix = child.split(rootPath).last();
            childFolderSuffixesOfRoot.insert(rootPath, suffix);

            QStringList childFiles = findNewFiles(child); // Find files in child folders of new root folders non-recursively.

            if(!childFiles.isEmpty())
            {
                for (qlonglong index = 0; index < childFiles.size(); ++index)
                    childFiles[index] = childFiles[index].split(child).last();

                filesObj.insert(child, QJsonArray::fromStringList(childFiles));
            }
        }
    }

    folders.append(rootFolders);

    std::sort(folders.begin(), folders.end(), [](const QString &s1, const QString &s2) {
        return s1.length() < s2.length();
    });

    QJsonObject responseBody;
    responseBody.insert("rootFolders", QJsonArray::fromStringList(rootFolders));
    responseBody.insert("folders", QJsonArray::fromStringList(folders));
    responseBody.insert("files", filesObj);

    QJsonObject suffixObj;
    for(const QString &rootPath : childFolderSuffixesOfRoot.uniqueKeys())
    {
        QStringList values = childFolderSuffixesOfRoot.values(rootPath);

        std::sort(values.begin(), values.end(), [](const QString &s1, const QString &s2) {
            return s1.length() < s2.length();
        });

        QJsonArray array = QJsonArray::fromStringList(values);
        suffixObj.insert(rootPath, array);
    }

    responseBody.insert("childFolderSuffixes", suffixObj);

    QJsonObject rootOfRootObj;
    for(const QString &rootPath : rootFolders)
    {
        QFileInfo info(rootPath.chopped(1));
        QString parentPath = QDir::toNativeSeparators(info.absolutePath());

        if(!parentPath.endsWith(QDir::separator()))
            parentPath.append(QDir::separator());

        if(QOperatingSystemVersion::currentType() == QOperatingSystemVersion::OSType::MacOS)
            parentPath = parentPath.normalized(QString::NormalizationForm::NormalizationForm_D);

        rootOfRootObj.insert(rootPath, parentPath);
    }

    responseBody.insert("rootOfRootFolder", rootOfRootObj);

    QHttpServerResponse response(responseBody, QHttpServerResponse::StatusCode::Ok);
    return response;
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

            // MacOS normalization
            //https://ss64.com/mac/syntax-filenames.html
            if(QOperatingSystemVersion::currentType() == QOperatingSystemVersion::OSType::MacOS)
                path = path.normalized(QString::NormalizationForm::NormalizationForm_D);

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

                if(QOperatingSystemVersion::currentType() == QOperatingSystemVersion::OSType::MacOS)
                    parentPath = parentPath.normalized(QString::NormalizationForm::NormalizationForm_D);

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

void RestController::newAddedList_findExisting(QSet<QString> &existingFolderSet, QSet<QString> &existingFileSet, QStringList &existingFolderList)
{
    auto fsm = FileStorageManager::instance();

    for(const QJsonValue &value : fsm->getActiveFolderList())
    {
        QString symbolPath = value[JsonKeys::Folder::SymbolFolderPath].toString();
        QString userPath = value[JsonKeys::Folder::UserFolderPath].toString();

        existingFolderSet.insert(userPath);
        existingFolderList.append(userPath);

        QJsonObject folderJson = fsm->getFolderJsonBySymbolPath(symbolPath, true);

        for(const QJsonValue &file : folderJson[JsonKeys::Folder::ChildFiles].toArray())
        {
            QString userFilePath = file[JsonKeys::File::UserFilePath].toString();
            existingFileSet.insert(userFilePath);
        }
    }

    std::sort(existingFolderList.begin(), existingFolderList.end(), [](const QString &s1, const QString &s2) {
        return s1.length() < s2.length();
    });
}

void RestController::newAddedList_findNewAtRootLevel(QSet<QString> existingFolderSet,
                                                     QSet<QString> existingFileSet,
                                                     QStringList existingFolderList,
                                                     QStringList &newRootFolderList,
                                                     QMultiHash<QString, QString> &newFileMap,
                                                     QHash<QString, QString> &newRootFolderRootMap)
{
    for(const QString &value : existingFolderList)
    {
        QDirIterator dirIterator(value, QDir::Filter::Files | QDir::Filter::Dirs | QDir::Filter::NoDotAndDotDot);

        while (dirIterator.hasNext())
        {
            QString path = QDir::toNativeSeparators(dirIterator.next());

            // MacOS normalization
            //https://ss64.com/mac/syntax-filenames.html
            if(QOperatingSystemVersion::currentType() == QOperatingSystemVersion::OSType::MacOS)
                path = path.normalized(QString::NormalizationForm::NormalizationForm_D);

            QFileInfo info = dirIterator.fileInfo();

            QString parentFolderPath = QDir::toNativeSeparators(info.absolutePath());

            if(QOperatingSystemVersion::currentType() == QOperatingSystemVersion::OSType::MacOS)
                parentFolderPath = parentFolderPath.normalized(QString::NormalizationForm::NormalizationForm_D);

            if(!parentFolderPath.endsWith(QDir::separator()))
                parentFolderPath.append(QDir::separator());

            if(info.isDir() && !path.endsWith(QDir::separator()))
                path.append(QDir::separator());

            if(info.isFile() && !existingFileSet.contains(path))
                newFileMap.insert(parentFolderPath, info.fileName());
            else if(info.isDir() && !existingFolderSet.contains(path))
            {
                newRootFolderList.append(path);
                newRootFolderRootMap.insert(path, parentFolderPath);
            }
        }
    }

    std::sort(newRootFolderList.begin(), newRootFolderList.end(), [](const QString &s1, const QString &s2) {
        return s1.length() < s2.length();
    });
}

void RestController::newAddedList_findChildrenOfRootFolders(QSet<QString> existingFolderSet,
                                                            QSet<QString> existingFileSet,
                                                            QStringList newRootFolderList,
                                                            QStringList &newFolderList,
                                                            QMultiHash<QString, QString> &childFoldersOfNewRootFolderMap,
                                                            QMultiHash<QString, QString> &newFileMap)
{
    QSet<QString> visitedFolderSet;

    for(const QString &value : newRootFolderList)
    {
        QDirIterator dirIterator(value,
                                 QDir::Filter::Files | QDir::Filter::Dirs | QDir::Filter::NoDotAndDotDot,
                                 QDirIterator::IteratorFlag::Subdirectories);

        while (dirIterator.hasNext())
        {
            QString path = QDir::toNativeSeparators(dirIterator.next());

            // MacOS normalization
            //https://ss64.com/mac/syntax-filenames.html
            if(QOperatingSystemVersion::currentType() == QOperatingSystemVersion::OSType::MacOS)
                path = path.normalized(QString::NormalizationForm::NormalizationForm_D);

            QFileInfo info = dirIterator.fileInfo();

            if(info.isDir() && !path.endsWith(QDir::separator()))
                path.append(QDir::separator());

            if(!visitedFolderSet.contains(path) && !existingFolderSet.contains(path) && !existingFileSet.contains(path))
            {
                if(info.isDir())
                {
                    newFolderList.append(path);
                    childFoldersOfNewRootFolderMap.insert(value, path);
                    visitedFolderSet.insert(path);
                }
                else if(info.isFile())
                {
                    QString parentPath = QDir::toNativeSeparators(info.absolutePath());

                    if(!parentPath.endsWith(QDir::separator()))
                        parentPath.append(QDir::separator());

                    if(QOperatingSystemVersion::currentType() == QOperatingSystemVersion::OSType::MacOS)
                        parentPath = parentPath.normalized(QString::NormalizationForm::NormalizationForm_D);

                    newFileMap.insert(parentPath, info.fileName());
                }
            }
        }
    }
}

QStringList RestController::generateRootFoldersList()
{
    QStringList result;

    auto fsm = FileStorageManager::instance();

    for(const QJsonValue &value : fsm->getActiveFolderList())
    {
        QString path = value.toObject()[JsonKeys::Folder::UserFolderPath].toString();
        QStringList childFolders = findNewFolders(path);

        if(!childFolders.isEmpty())
            result.append(childFolders);
    }

    return result;
}

QJsonObject RestController::generateChildFolderSuffixObject(QStringList rootFolderList)
{
    QJsonObject result;

    for(const QString &rootPath : rootFolderList)
    {
        QStringList childFolders = findNewFolders(rootPath, true);
        QStringList suffixList;

        for(const QString &child : childFolders)
        {
            QString suffix = child.split(rootPath).last();
            suffixList.append(suffix);
        }

        std::sort(suffixList.begin(), suffixList.end(), [](const QString &s1, const QString &s2) {
            return s1.length() < s2.length();
        });

        result.insert(rootPath, QJsonArray::fromStringList(suffixList));
    }

    return result;
}

QJsonObject RestController::generateRootOfRootFoldersObject(QStringList rootFolderList)
{
    QJsonObject result;

    for(const QString &rootPath : rootFolderList)
    {
        QFileInfo info(rootPath.chopped(1));
        QString parentPath = QDir::toNativeSeparators(info.absolutePath());

        if(!parentPath.endsWith(QDir::separator()))
            parentPath.append(QDir::separator());

        if(QOperatingSystemVersion::currentType() == QOperatingSystemVersion::OSType::MacOS)
            parentPath = parentPath.normalized(QString::NormalizationForm::NormalizationForm_D);

        result.insert(rootPath, parentPath);
    }

    return result;
}

QStringList RestController::generateFoldersList()
{
    QStringList result;

    auto fsm = FileStorageManager::instance();

    for(const QJsonValue &value : fsm->getActiveFolderList())
    {
        QString path = value.toObject()[JsonKeys::Folder::UserFolderPath].toString();
        QStringList childFolders = findNewFolders(path, true);

        if(!childFolders.isEmpty())
            result.append(childFolders);
    }

    std::sort(result.begin(), result.end(), [](const QString &s1, const QString &s2) {
        return s1.length() < s2.length();
    });

    return result;
}

QJsonObject RestController::generateFilesObject()
{
    QJsonObject result;
    QMultiHash<QString, QString> fileMap;

    auto fsm = FileStorageManager::instance();

    for(const QJsonValue &value : fsm->getActiveFolderList())
    {
        QString folderPath = value.toObject()[JsonKeys::Folder::UserFolderPath].toString();
        QStringList childFiles = findNewFiles(folderPath, true);

        if(!childFiles.isEmpty())
        {
            QString parentFolderPath;

            for (qlonglong index = 0; index < childFiles.size(); ++index)
            {
                parentFolderPath = QFileInfo(childFiles[index]).absolutePath();

                if(QOperatingSystemVersion::currentType() == QOperatingSystemVersion::OSType::MacOS)
                    parentFolderPath = parentFolderPath.normalized(QString::NormalizationForm::NormalizationForm_D);

                if(!parentFolderPath.endsWith(QDir::separator()))
                    parentFolderPath.append(QDir::separator());

                QString fileName = childFiles[index].split(parentFolderPath).last();
                fileMap.insert(parentFolderPath, fileName);
            }
        }
    }

    for(const QString &folderPath : fileMap.uniqueKeys())
    {
        QStringList values = fileMap.values(folderPath);
        QSet<QString> uniqueValues(values.begin(), values.end()); // Remove duplicate file names.

        result.insert(folderPath, QJsonArray::fromStringList(uniqueValues.values()));
    }

    return result;
}

QStringList RestController::findNewFolders(const QString &rootPath, bool isRecursive)
{
    auto fsm = FileStorageManager::instance();
    QStringList result;

    QScopedPointer<QDirIterator>ptr(new QDirIterator(rootPath, QDir::Filter::Dirs | QDir::Filter::NoDotAndDotDot));

    if(isRecursive)
    {
        auto rawPtr = new QDirIterator(rootPath,
                                       QDir::Filter::Dirs | QDir::Filter::NoDotAndDotDot,
                                       QDirIterator::IteratorFlag::Subdirectories);
        ptr.reset(rawPtr);
    }

    while (ptr->hasNext())
    {
        QString path = QDir::toNativeSeparators(ptr->next());

        // MacOS normalization
        //https://ss64.com/mac/syntax-filenames.html
        if(QOperatingSystemVersion::currentType() == QOperatingSystemVersion::OSType::MacOS)
            path = path.normalized(QString::NormalizationForm::NormalizationForm_D);

        if(!path.endsWith(QDir::separator()))
            path.append(QDir::separator());

        bool isExists = fsm->getFolderJsonByUserPath(path)[JsonKeys::IsExist].toBool();

        if(!isExists)
            result.append(path);
    }

    return result;
}

QStringList RestController::findNewFiles(const QString &rootPath, bool isRecursive)
{
    auto fsm = FileStorageManager::instance();
    QStringList result;

    QScopedPointer<QDirIterator>ptr(new QDirIterator(rootPath, QDir::Filter::Files | QDir::Filter::NoDotAndDotDot));

    if(isRecursive)
    {
        auto rawPtr = new QDirIterator(rootPath,
                                       QDir::Filter::Files | QDir::Filter::NoDotAndDotDot,
                                       QDirIterator::IteratorFlag::Subdirectories);
        ptr.reset(rawPtr);
    }

    while (ptr->hasNext())
    {
        QString path = QDir::toNativeSeparators(ptr->next());

        // MacOS normalization
        //https://ss64.com/mac/syntax-filenames.html
        if(QOperatingSystemVersion::currentType() == QOperatingSystemVersion::OSType::MacOS)
            path = path.normalized(QString::NormalizationForm::NormalizationForm_D);

        bool isExists = fsm->getFileJsonByUserPath(path)[JsonKeys::IsExist].toBool();

        if(!isExists)
            result.append(path);
    }

    return result;
}
