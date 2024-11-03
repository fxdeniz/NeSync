#include "ZipExportController.h"

#include "JsonDtoFormat.h"
#include "FileStorageSubSystem/FileStorageManager.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QOperatingSystemVersion>
#include <quazip/quazip.h>
#include <quazip/quazipfile.h>


ZipExportController::ZipExportController(QObject *parent)
    : QObject{parent}
{}

QHttpServerResponse ZipExportController::postSetZipFilePath(const QHttpServerRequest &request)
{
    QByteArray requestBody = request.body();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(requestBody);
    QJsonObject jsonObject = jsonDoc.object();

    QString filePath = jsonObject["filePath"].toString();

    if(QOperatingSystemVersion::currentType() == QOperatingSystemVersion::OSType::MacOS)
        filePath = filePath.normalized(QString::NormalizationForm::NormalizationForm_D);

    qDebug() << "filePath = " << filePath;
    qDebug() << "";

    this->zipFilePath = filePath;

    return QHttpServerResponse(QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse ZipExportController::getZipFilePath(const QHttpServerRequest &request)
{
    QJsonObject responseBody {{"filePath", this->zipFilePath}};
    QHttpServerResponse response = QHttpServerResponse(responseBody, QHttpServerResponse::StatusCode::Ok);

    return response;
}

QHttpServerResponse ZipExportController::postSetRootSymbolFolderPath(const QHttpServerRequest &request)
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

    this->rootSymbolFolderPath = rootSymbolFolderPath;

    return QHttpServerResponse(QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse ZipExportController::getRootSymbolFolderPath(const QHttpServerRequest &request)
{
    QJsonObject responseBody {{"rootPath", this->rootSymbolFolderPath}};
    QHttpServerResponse response = QHttpServerResponse(responseBody, QHttpServerResponse::StatusCode::Ok);

    return response;
}

QHttpServerResponse ZipExportController::postCreateArchive(const QHttpServerRequest &request)
{
    QuaZip archive(this->zipFilePath);
    bool isCreated = archive.open(QuaZip::Mode::mdCreate);

    QJsonObject responseBody {{"isCreated", isCreated}};
    QHttpServerResponse response = QHttpServerResponse(responseBody, QHttpServerResponse::StatusCode::Ok);

    return response;
}

QHttpServerResponse ZipExportController::postAddFolderJson(const QHttpServerRequest &request)
{
    auto fsm = FileStorageManager::instance();

    QList<QJsonObject> folderStack;
    folderStack.append(fsm->getFolderJsonBySymbolPath(this->rootSymbolFolderPath));

    QStringList folderJsonContent;

    while(!folderStack.isEmpty())
    {
        QJsonObject currentFolder = folderStack.first();
        folderStack.removeFirst();

        folderJsonContent.append(currentFolder[JsonKeys::Folder::SymbolFolderPath].toString());

        currentFolder = fsm->getFolderJsonBySymbolPath(currentFolder[JsonKeys::Folder::SymbolFolderPath].toString(), true);

        for(const QJsonValue &value : currentFolder[JsonKeys::Folder::ChildFolders].toArray())
            folderStack.append(value.toObject());
    }

    QuaZip archive(this->zipFilePath);
    bool isArchiveOpened = archive.open(QuaZip::Mode::mdAdd);

    QuaZipFile foldersJsonFile(&archive);
    bool isFileOpened = foldersJsonFile.open(QFile::OpenModeFlag::WriteOnly, QuaZipNewInfo("folders.json"));

    if(!isArchiveOpened || !isFileOpened)
    {
        QJsonObject responseBody {{"isAdded", false}};
        return QHttpServerResponse(responseBody, QHttpServerResponse::StatusCode::Ok);
    }

    std::sort(folderJsonContent.begin(), folderJsonContent.end(), [](const QString &s1, const QString &s2) {
        return s1.length() < s2.length();
    });

    QJsonDocument document(QJsonArray::fromStringList(folderJsonContent));
    qint64 bytesWritten = foldersJsonFile.write(document.toJson(QJsonDocument::JsonFormat::Indented));

    QJsonObject responseBody {{"isAdded", true}};

    if(bytesWritten <= -1)
        responseBody.insert("isAdded", false);

    return QHttpServerResponse(responseBody, QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse ZipExportController::postAddFileJson(const QHttpServerRequest &request)
{
    auto fsm = FileStorageManager::instance();

    QList<QJsonObject> folderStack;
    folderStack.append(fsm->getFolderJsonBySymbolPath(this->rootSymbolFolderPath));

    QJsonObject filesJsonContent;

    while(!folderStack.isEmpty())
    {
        QJsonObject currentFolder = folderStack.first();
        folderStack.removeFirst();

        currentFolder = fsm->getFolderJsonBySymbolPath(currentFolder[JsonKeys::Folder::SymbolFolderPath].toString(), true);

        for(const QJsonValue &value : currentFolder[JsonKeys::Folder::ChildFolders].toArray())
            folderStack.append(value.toObject());

        for(const QJsonValue &value : currentFolder[JsonKeys::Folder::ChildFiles].toArray())
        {
            QJsonObject currentFile = fsm->getFileJsonBySymbolPath(value.toObject()[JsonKeys::File::SymbolFilePath].toString(), true);
            currentFile.remove(JsonKeys::IsExist);
            currentFile.remove(JsonKeys::File::IsFrozen);
            currentFile.remove(JsonKeys::File::UserFilePath);

            QJsonArray versionList = currentFile[JsonKeys::File::VersionList].toArray();

            for (qlonglong index = 0; index < versionList.size(); ++index)
            {
                QJsonObject version = versionList[index].toObject();
                version.remove(JsonKeys::IsExist);
                version.remove(JsonKeys::FileVersion::NewVersionNumber);
                versionList[index] = version;
            }

            currentFile[JsonKeys::File::VersionList] = versionList;

            filesJsonContent.insert(currentFile[JsonKeys::File::SymbolFilePath].toString(), currentFile);
        }
    }

    QuaZip archive(this->zipFilePath);
    bool isArchiveOpened = archive.open(QuaZip::Mode::mdAdd);

    QuaZipFile filesJsonFile(&archive);
    bool isFileOpened = filesJsonFile.open(QFile::OpenModeFlag::WriteOnly, QuaZipNewInfo("files.json"));

    if(!isArchiveOpened || !isFileOpened)
    {
        QJsonObject responseBody {{"isAdded", false}};
        return QHttpServerResponse(responseBody, QHttpServerResponse::StatusCode::Ok);
    }

    QJsonDocument document(filesJsonContent);
    qint64 bytesWritten = filesJsonFile.write(document.toJson(QJsonDocument::JsonFormat::Indented));

    QJsonObject responseBody {{"isAdded", true}};
    responseBody.insert("files", filesJsonContent);
    setFilesJson(filesJsonContent);

    if(bytesWritten <= -1)
    {
        responseBody.insert("isAdded", false);
        responseBody.insert("files", {});
        setFilesJson({});
    }

    return QHttpServerResponse(responseBody, QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse ZipExportController::postAddFileToZip(const QHttpServerRequest &request)
{
    QByteArray requestBody = request.body();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(requestBody);
    QJsonObject jsonObject = jsonDoc.object();

    QString symbolFilePath = jsonObject["symbolFilePath"].toString();
    qlonglong versionNumber = jsonObject["versionNumber"].toInteger();

    qDebug() << "symbolFilePath = " << symbolFilePath;
    qDebug() << "versionNumber = " << versionNumber;
    qDebug() << "";

    QJsonObject file = getFilesJson()[symbolFilePath].toObject();
    QJsonObject version;

    // TODO: Find something more efficent than traversing and searching the version number.
    for(const QJsonValue &value : file[JsonKeys::File::VersionList].toArray())
    {
        QJsonObject current = value.toObject();
        qlonglong number = value[JsonKeys::FileVersion::VersionNumber].toInteger();

        if(versionNumber == number)
        {
            version = current;
            break;
        }
    }

    QuaZip archive(this->zipFilePath);
    bool isArchiveOpened = archive.open(QuaZip::Mode::mdAdd);

    if(!isArchiveOpened)
    {
        QJsonObject responseBody {{"isAdded", false}};
        return QHttpServerResponse(responseBody, QHttpServerResponse::StatusCode::Ok);
    }

    auto fsm = FileStorageManager::instance();

    QString internalFileName = version[JsonKeys::FileVersion::InternalFileName].toString();
    QString internalFilePath = fsm->getStorageFolderPath() + internalFileName;

    QFile rawFile(internalFilePath);
    bool isReadable = rawFile.open(QFile::OpenModeFlag::ReadOnly);

    if(!isReadable)
    {
        QJsonObject responseBody {{"isAdded", false}};
        return QHttpServerResponse(responseBody, QHttpServerResponse::StatusCode::Ok);
    }

    QuaZipNewInfo info(internalFileName, internalFilePath);
    QuaZipFile fileInZip(&archive);
    fileInZip.open(QFile::OpenModeFlag::WriteOnly, info);

    while(!rawFile.atEnd())
    {
        // Write up to 100mb in every iteration.
        qlonglong bytesWritten = fileInZip.write(rawFile.read(104857600));
        if(bytesWritten == -1)
        {
            QJsonObject responseBody {{"isAdded", false}};
            return QHttpServerResponse(responseBody, QHttpServerResponse::StatusCode::Ok);
        }
    }

    QJsonObject responseBody {{"isAdded", true}};
    return QHttpServerResponse(responseBody, QHttpServerResponse::StatusCode::Ok);
}

QJsonObject ZipExportController::getFilesJson() const
{
    return filesJson;
}

void ZipExportController::setFilesJson(const QJsonObject &newFileJson)
{
    filesJson = newFileJson;
}
