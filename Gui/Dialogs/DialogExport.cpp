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
    });

    QObject::connect(this, &DialogExport::signalZippingFinished, this, [=]{
        ui->buttonExport->setEnabled(true);
        ui->buttonSelectLocation->setEnabled(true);
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
    ui->buttonExport->setDisabled(true);
    ui->progressBar->setValue(0);
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
}


void DialogExport::on_buttonExport_clicked()
{
    ui->buttonExport->setDisabled(true);
    ui->buttonSelectLocation->setDisabled(true);

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

            for(const QJsonValue &currentFileVersion : qAsConst(versionJsonArray))
            {
                QJsonObject versionJson = currentFileVersion.toObject();
                QString internalFileName = versionJson[JsonKeys::FileVersion::InternalFileName].toString();
                QString internalFilePath = fsm->getBackupFolderPath() + internalFileName;

                QFile rawFile(internalFilePath);
                rawFile.open(QFile::OpenModeFlag::ReadOnly);

                QuaZipNewInfo info(internalFileName, internalFilePath);
                QuaZipFile fileInZip(&archive);
                fileInZip.open(QFile::OpenModeFlag::WriteOnly, info);

                while(!rawFile.atEnd())
                    fileInZip.write(rawFile.read(104857600)); // Read up to 100mb in every iteration.

                ++zipProgressValue;
                emit signalZipProgressUpdated(zipProgressValue);
            }
        }

        emit signalZippingFinished();
    });
}

