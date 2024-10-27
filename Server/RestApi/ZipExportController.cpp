#include "ZipExportController.h"

#include "JsonDtoFormat.h"
#include "FileStorageSubSystem/FileStorageManager.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <quazip/quazip.h>
#include <quazip/quazipfile.h>


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

    setZipFilePath(folderPath + fileName);

    QuaZip archive(getZipFilePath());
    bool isCreated = archive.open(QuaZip::Mode::mdCreate);

    QJsonObject responseBody {{"isCreated", isCreated}};
    QHttpServerResponse response = QHttpServerResponse(responseBody, QHttpServerResponse::StatusCode::Ok);

    return response;
}

QHttpServerResponse ZipExportController::postAddFolderJson(const QHttpServerRequest &request)
{
    auto fsm = FileStorageManager::instance();

    QList<QJsonObject> folderStack;
    folderStack.append(fsm->getFolderJsonBySymbolPath("/"));

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

    QuaZip archive(getZipFilePath());
    bool isArchiveOpened = archive.open(QuaZip::Mode::mdAppend);

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

QString ZipExportController::getZipFilePath() const
{
    return zipFilePath;
}

void ZipExportController::setZipFilePath(const QString &newZipFilePath)
{
    zipFilePath = newZipFilePath;
}
