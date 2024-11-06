#include "FileSystemMonitorController.h"

#include "JsonDtoFormat.h"
#include "FileStorageSubSystem/FileStorageManager.h"

#include <QJsonObject>
#include <QDirIterator>
#include <QJsonDocument>
#include <QOperatingSystemVersion>

FileSystemMonitorController::FileSystemMonitorController(QObject *parent)
    : QObject{parent}
{}

QHttpServerResponse FileSystemMonitorController::newAddedList(const QHttpServerRequest &request)
{
    QStringList rootFolderList = generateRootFoldersList();
    QJsonObject responseBody;

    responseBody.insert("rootFolders", QJsonArray::fromStringList(rootFolderList));
    responseBody.insert("childFolderSuffixes", generateChildFolderSuffixObject(rootFolderList));
    responseBody.insert("rootOfRootFolder", generateRootOfRootFoldersObject(rootFolderList));
    responseBody.insert("files", generateFilesObject(rootFolderList));
    responseBody.insert("folders", QJsonArray::fromStringList(generateFoldersList(rootFolderList)));

    QHttpServerResponse response(responseBody, QHttpServerResponse::StatusCode::Ok);
    return response;
}

QHttpServerResponse FileSystemMonitorController::deletedList(const QHttpServerRequest &request)
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

QHttpServerResponse FileSystemMonitorController::updatedFileList(const QHttpServerRequest &request)
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

QStringList FileSystemMonitorController::generateRootFoldersList()
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

QJsonObject FileSystemMonitorController::generateChildFolderSuffixObject(QStringList rootFolderList)
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

        if(!childFolders.isEmpty())
            result.insert(rootPath, QJsonArray::fromStringList(suffixList));
    }

    return result;
}

QJsonObject FileSystemMonitorController::generateRootOfRootFoldersObject(QStringList rootFolderList)
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

QJsonObject FileSystemMonitorController::generateFilesObject(QStringList rootFolderList)
{
    QJsonObject result;
    QMultiHash<QString, QString> fileMap;

    auto fsm = FileStorageManager::instance();

    // Find new files in existing folders.
    for(const QJsonValue &value : fsm->getActiveFolderList())
    {
        QString folderPath = value.toObject()[JsonKeys::Folder::UserFolderPath].toString();
        QStringList childFiles = findNewFiles(folderPath);

        if(!childFiles.isEmpty())
        {
            QString parentFolderPath;

            for (qulonglong index = 0; index < childFiles.size(); ++index)
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

    // Find new files in new root folders.
    for(const QString &rootPath : rootFolderList)
    {
        QStringList childFiles = findNewFiles(rootPath, true); // This step is recursive, in the previous step it is not.

        if(!childFiles.isEmpty())
        {
            QString parentFolderPath;

            for (qulonglong index = 0; index < childFiles.size(); ++index)
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

        result.insert(folderPath, QJsonArray::fromStringList(values));
    }

    return result;
}

QStringList FileSystemMonitorController::generateFoldersList(QStringList rootFolderList)
{
    QStringList result;

    for(const QString &rootPath : rootFolderList)
    {
        QStringList childFolders = findNewFolders(rootPath, true);

        if(!childFolders.isEmpty())
            result.append(childFolders);
    }

    result.append(rootFolderList);

    std::sort(result.begin(), result.end(), [](const QString &s1, const QString &s2) {
        return s1.length() < s2.length();
    });

    return result;
}

QStringList FileSystemMonitorController::findNewFolders(const QString &rootPath, bool isRecursive)
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

QStringList FileSystemMonitorController::findNewFiles(const QString &rootPath, bool isRecursive)
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
