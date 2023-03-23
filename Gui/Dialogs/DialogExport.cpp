#include "DialogExport.h"
#include "ui_DialogExport.h"
#include "Utility/JsonDtoFormat.h"
#include "Backend/FileStorageSubSystem/FileStorageManager.h"

#include <quazip/quazip.h>
#include <quazip/quazipfile.h>

#include <QQueue>
#include <QFileDialog>
#include <QJsonObject>
#include <QtConcurrent>
#include <QJsonDocument>
#include <QStandardPaths>

DialogExport::DialogExport(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogExport)
{
    ui->setupUi(this);

    QObject::connect(this, &DialogExport::signalZipProgressUpdated,
                     ui->progressBar, &QProgressBar::setValue);

    QObject::connect(this, &DialogExport::signalZipCreationStarted, this, [=] (int minimum, int maximum) {
        ui->progressBar->setMinimum(minimum);
        ui->progressBar->setMaximum(maximum);
        ui->labelFileStatus->clear();
        ui->labelFilePath->clear();
    });

    QObject::connect(this, &DialogExport::signalAddingFileToZip, this, [=](const QString &symbolFilePath){
        ui->labelFileStatus->setText(tr("Adding file:"));
        ui->labelFilePath->setText("<b>" + symbolFilePath + "</b>");
    });

    QObject::connect(this, &DialogExport::signalZippingFinished, this, [=](bool finishedSuccessfully){
        if(finishedSuccessfully)
        {
            ui->buttonSelectLocation->setEnabled(true);
            ui->labelFileStatus->clear();
            ui->labelFilePath->clear();
            showStatusSuccess(statusTextZippingFinishedWithoutError(), ui->labelStatus);
        }
        else
        {
            showStatusError(statusTextZippingFinishedWithError(), ui->labelStatus);
            ui->labelFileStatus->setText(tr("Stopped at:"));
        }
    });
}

DialogExport::~DialogExport()
{
    delete ui;
}

void DialogExport::show(QList<QString> itemList)
{
    this->itemList = itemList;
    ui->lineEdit->clear();
    ui->buttonSelectLocation->setEnabled(true);
    ui->buttonExport->setDisabled(true);
    ui->progressBar->setValue(0);
    ui->labelFileStatus->clear();
    ui->labelFilePath->clear();
    showStatusInfo(statusTextSelectLocation(), ui->labelStatus);
    QWidget::show();
}

void DialogExport::on_buttonSelectLocation_clicked()
{
    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::StandardLocation::DesktopLocation);
    desktopPath = QDir::toNativeSeparators(desktopPath);
    desktopPath += QDir::separator();

    QString filePath = QFileDialog::getSaveFileName(this, tr("Save Zip File"),
                                                    desktopPath,
                                                    tr("Zip files (*.zip)"));

    if(filePath.isEmpty())
        return;

    ui->lineEdit->setText(filePath);
    ui->buttonExport->setEnabled(true);
    showStatusSuccess(statusTextZipFileReadyToCreate(), ui->labelStatus);
}


void DialogExport::on_buttonExport_clicked()
{
    ui->buttonExport->setDisabled(true);
    ui->buttonSelectLocation->setDisabled(true);
    showStatusInfo(statusTextZippingInProgress(), ui->labelStatus);
    ui->progressBar->setValue(0);

    QFuture<void> future = QtConcurrent::run([=]{
        QJsonArray fileJsonArray;
        qlonglong totalFileCount = 0;
        auto fsm = FileStorageManager::instance();

        for(const QString &currentSymbolPath : itemList)
        {
            QJsonObject currentJson = fsm->getFileJsonBySymbolPath(currentSymbolPath, true);

            if(currentJson[JsonKeys::IsExist].toBool())
            {
                fileJsonArray.append(currentJson);
                totalFileCount += currentJson[JsonKeys::File::VersionList].toArray().size();
                continue;
            }

            currentJson = fsm->getFolderJsonBySymbolPath(currentSymbolPath, true);

            if(currentJson[JsonKeys::IsExist].toBool())
            {
                QQueue<QJsonObject> folders;
                folders.enqueue(currentJson);

                while(!folders.isEmpty())
                {
                    QJsonObject currentFolder = folders.dequeue();
                    QString currentFolderPath = currentFolder[JsonKeys::Folder::SymbolFolderPath].toString();
                    QJsonArray childFolders = currentFolder[JsonKeys::Folder::ChildFolders].toArray();
                    for(const QJsonValue &currentChildFolder : childFolders)
                        folders.enqueue(currentChildFolder.toObject());

                    currentFolder = fsm->getFolderJsonBySymbolPath(currentFolderPath, true);

                    QJsonArray childFiles = currentFolder[JsonKeys::Folder::ChildFiles].toArray();
                    for(const QJsonValue &currentChildFile : childFiles)
                    {
                        QJsonObject childFileJson = currentChildFile.toObject();
                        QString currentFileSymbolPath = childFileJson[JsonKeys::File::SymbolFilePath].toString();
                        childFileJson = fsm->getFileJsonBySymbolPath(currentFileSymbolPath, true);
                        fileJsonArray.append(childFileJson);
                        totalFileCount += childFileJson[JsonKeys::File::VersionList].toArray().size();
                    }
                }
            }
        }

        emit signalZipCreationStarted(0, totalFileCount);

        QuaZip archive(ui->lineEdit->text());
        archive.open(QuaZip::Mode::mdCreate);

        QuaZipFile importJsonFile(&archive);
        importJsonFile.open(QFile::OpenModeFlag::WriteOnly, QuaZipNewInfo("import.json"));

        QJsonDocument document(fileJsonArray);
        importJsonFile.write(document.toJson(QJsonDocument::JsonFormat::Indented));

        int zipProgressValue = 0;
        for(const QJsonValue &currentFileJson : qAsConst(fileJsonArray))
        {
            QJsonObject fileJson = currentFileJson.toObject();
            QJsonArray versionJsonArray = fileJson[JsonKeys::File::VersionList].toArray();
            emit signalAddingFileToZip(fileJson[JsonKeys::File::SymbolFilePath].toString());

            for(const QJsonValue &currentFileVersion : qAsConst(versionJsonArray))
            {
                QJsonObject versionJson = currentFileVersion.toObject();
                QString internalFileName = versionJson[JsonKeys::FileVersion::InternalFileName].toString();
                QString internalFilePath = fsm->getBackupFolderPath() + internalFileName;

                QFile rawFile(internalFilePath);
                bool isReadable = rawFile.open(QFile::OpenModeFlag::ReadOnly);

                if(!isReadable)
                {
                    emit signalZippingFinished(false);
                    return;
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
                        emit signalZippingFinished(false);
                        return;
                    }
                }

                ++zipProgressValue;
                emit signalZipProgressUpdated(zipProgressValue);
            }
        }

        emit signalZippingFinished(true);
    });
}

QString DialogExport::statusTextSelectLocation()
{
    return tr("Please select location for zip file where it will be created");
}

QString DialogExport::statusTextZipFileReadyToCreate()
{
    return tr("Zip file is ready to create");
}

QString DialogExport::statusTextZippingInProgress()
{
    return tr("Your files are being compressed in the background...");
}

QString DialogExport::statusTextZippingFinishedWithoutError()
{
    return tr("Zip file created successfully");
}

QString DialogExport::statusTextZippingFinishedWithError()
{
    return tr("Zip file creation canceled due to error");
}

