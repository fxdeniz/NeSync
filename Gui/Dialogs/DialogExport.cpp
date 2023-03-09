#include "DialogExport.h"
#include "ui_DialogExport.h"
#include "Utility/JsonDtoFormat.h"
#include "Backend/FileStorageSubSystem/FileStorageManager.h"

#include <QQueue>
#include <QFileDialog>
#include <QJsonObject>
#include <QJsonDocument>
#include <QStandardPaths>

DialogExport::DialogExport(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogExport)
{
    ui->setupUi(this);
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
    QMap<QString, QJsonObject> fileMap;
    auto fsm = FileStorageManager::instance();

    for(const QString &currentSymbolPath : itemList)
    {
        QJsonObject currentJson = fsm->getFileJsonBySymbolPath(currentSymbolPath, true);

        if(currentJson[JsonKeys::IsExist].toBool())
        {
            fileMap.insert(currentSymbolPath, currentJson);
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
                    QJsonObject childFile = currentChildFile.toObject();
                    QString currentFileSymbolPath = childFile[JsonKeys::File::SymbolFilePath].toString();
                    childFile = fsm->getFileJsonBySymbolPath(currentFileSymbolPath, true);
                    fileMap.insert(currentFileSymbolPath, childFile);
                }
            }
        }
    }

    QJsonDocument document;
    QMapIterator<QString, QJsonObject> mapIterator(fileMap);

    QString filePath = QStandardPaths::writableLocation(QStandardPaths::StandardLocation::TempLocation);
    filePath.append(QDir::separator());
    filePath = QDir::toNativeSeparators(filePath);
    filePath += QUuid::createUuid().toString(QUuid::StringFormat::Id128);
    filePath += ".json";

    QFile file(filePath);
    file.open(QFile::OpenModeFlag::WriteOnly);
    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Encoding::Utf8);

    while(mapIterator.hasNext())
    {
        mapIterator.next();
        stream << "";
        //stream << "file " << mapIterator.key() << " has: ";
        document.setObject(mapIterator.value());
        stream << document.toJson();
        stream << "";
    }
}

