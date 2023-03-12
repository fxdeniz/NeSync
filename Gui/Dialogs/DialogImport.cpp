#include "DialogImport.h"
#include "ui_DialogImport.h"
#include "Utility/JsonDtoFormat.h"
#include "DataModels/DialogImport/TreeModelDialogImport.h"
#include "Backend/FileStorageSubSystem/FileStorageManager.h"

#include <quazip/quazip.h>
#include <quazip/quazipfile.h>
#include <QDataStream>
#include <QFileDialog>
#include <QJsonObject>
#include <QJsonDocument>
#include <QStandardPaths>
#include <QTemporaryFile>

DialogImport::DialogImport(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogImport)
{
    ui->setupUi(this);
    itemDelegateAction = new TreeModelDialogImport::ItemDelegateAction(this);
}

DialogImport::~DialogImport()
{
    delete ui;
}

void DialogImport::show()
{
    showStatusInfo(statusTextWaitingForFile(), ui->labelStatus);
    ui->lineEdit->clear();
    ui->buttonImport->setDisabled(true);
    QWidget::show();
}

void DialogImport::on_buttonSelectFile_clicked()
{
    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::StandardLocation::DesktopLocation);
    desktopPath = QDir::toNativeSeparators(desktopPath) + QDir::separator();

    QString importFilePath = QFileDialog::getOpenFileName(this, tr("Open a Zip file"),
                                                                desktopPath,
                                                                tr("Zip Files (*.zip)"));

    if(importFilePath.isEmpty())
        return;

    ui->lineEdit->setText(importFilePath);

    QuaZip archive(ui->lineEdit->text());
    bool isArchiveOpened = archive.open(QuaZip::mdUnzip);

    if(!isArchiveOpened)
    {
        QFileInfo info(ui->lineEdit->text());
        showStatusError(statusTextCanNotOpenFile(info.fileName()), ui->labelStatus);
        return;
    }

    QString importFileName = "import.json";
    bool isFileFound = archive.setCurrentFile(importFileName);

    if(!isFileFound)
    {
        showStatusError(statusTextImportJsonFileMissing(), ui->labelStatus);
        return;
    }

    QuaZipFile importJsonFile(&archive);
    bool isFileOpened = importJsonFile.open(QFile::OpenModeFlag::ReadOnly);

    if(!isFileOpened)
    {
        showStatusError(statusTextCanNotOpenFile(importFileName), ui->labelStatus);
        return;
    }

    QJsonParseError parseError;
    QJsonDocument document = QJsonDocument::fromJson(importJsonFile.readAll(), &parseError);

    if(parseError.error != QJsonParseError::ParseError::NoError || !document.isArray())
    {
        showStatusError(statusTextImportJsonFileCorrupt(), ui->labelStatus);
        return;
    }

    // TODO: add json schema validation

    showStatusSuccess(statusTextFileReadyToImport(), ui->labelStatus);
    ui->buttonImport->setEnabled(true);

    if(ui->treeView->model() != nullptr)
        delete ui->treeView->model();

    auto treeModel = new TreeModelDialogImport::Model(document.array(), ui->treeView);
    ui->treeView->setModel(treeModel);

    ui->treeView->header()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
    ui->treeView->header()->setSectionResizeMode(TreeModelDialogImport::Model::ColumnIndexSymbolPath,
                                                 QHeaderView::ResizeMode::Interactive);
    ui->treeView->header()->setMinimumSectionSize(200);
    ui->treeView->setColumnWidth(TreeModelDialogImport::Model::ColumnIndexSymbolPath, 500);

    ui->treeView->setSelectionMode(QAbstractItemView::SelectionMode::ContiguousSelection);
    ui->treeView->setItemDelegateForColumn(TreeModelDialogImport::Model::ColumnIndexAction, itemDelegateAction);
    ui->treeView->expandAll();
    ui->treeView->selectAll();

    QItemSelectionModel *selectionModel = ui->treeView->selectionModel();
    for(const QModelIndex &index : selectionModel->selectedRows(TreeModelDialogImport::Model::ColumnIndexAction))
        ui->treeView->openPersistentEditor(index);

    ui->treeView->clearSelection();
}

void DialogImport::on_buttonImport_clicked()
{
    auto treeModel = (TreeModelDialogImport::Model *) ui->treeView->model();
    treeModel->disableComboBoxes();

    auto fsm = FileStorageManager::instance();
    QuaZip archive(ui->lineEdit->text());
    archive.open(QuaZip::Mode::mdUnzip);
    QMapIterator<QString, TreeModelDialogImport::TreeItem *> mapIterator(treeModel->getFolderItemMap());

    while(mapIterator.hasNext())
    {
        mapIterator.next();

        TreeModelDialogImport::TreeItem *folderItem = mapIterator.value();
        if(folderItem->getAction() == TreeModelDialogImport::TreeItem::DoNotImport)
            continue;

        fsm->addNewFolder(mapIterator.key(), "");

        for(int index = 0; index < folderItem->childCount(); index++)
        {
            bool addingFirstVersion = true;
            TreeModelDialogImport::TreeItem *childFileItem = folderItem->child(index);
            if(childFileItem->getAction() == TreeModelDialogImport::TreeItem::Action::Import ||
               childFileItem->getAction() == TreeModelDialogImport::TreeItem::Action::Overwrite)
            {
                QString symbolFilePath = childFileItem->getFileJson()[JsonKeys::File::SymbolFilePath].toString();
                QJsonObject previousFile = fsm->getFileJsonBySymbolPath(symbolFilePath);
                fsm->deleteFile(symbolFilePath);

                if(!previousFile[JsonKeys::File::IsFrozen].toBool())
                    emit signalFileImportStarted(previousFile[JsonKeys::File::UserFilePath].toString());

                QJsonArray versionList = childFileItem->getFileJson()[JsonKeys::File::VersionList].toArray();
                for(const QJsonValue &currentValue : versionList)
                {
                    QJsonObject versionJson = currentValue.toObject();
                    archive.setCurrentFile(versionJson[JsonKeys::FileVersion::InternalFileName].toString());

                    QuaZipFile fileInZip(&archive);
                    fileInZip.open(QFile::OpenModeFlag::ReadOnly);
                    QTemporaryFile tempFile;
                    tempFile.open();

                    while(!fileInZip.atEnd())
                    {
                        tempFile.write(fileInZip.read(104857600)); // Read up to 100mb.
                        tempFile.flush();
                    }

                    QString description = versionJson[JsonKeys::FileVersion::Description].toString();

                    if(!addingFirstVersion)
                        fsm->appendVersion(symbolFilePath, tempFile.fileName(), description);
                    else
                    {
                        fsm->addNewFile(childFileItem->getFileJson()[JsonKeys::File::SymbolFolderPath].toString(),
                                        tempFile.fileName(),
                                        true,
                                        childFileItem->getName(),
                                        description);
                    }

                    addingFirstVersion = false;
                }
            }
        }
    }
}

QString DialogImport::statusTextWaitingForFile()
{
    return tr("Please select a zip file");
}

QString DialogImport::statusTextCanNotOpenFile(const QString &fileNameArg)
{
    return tr("Couldn't open selected file: <b>%1</b>").arg(fileNameArg);
}

QString DialogImport::statusTextImportJsonFileMissing()
{
    return tr("<b>import.json</b> file not exist in selected zip file");
}

QString DialogImport::statusTextImportJsonFileCorrupt()
{
    return tr("<b>import.json</b> is corrupt");
}

QString DialogImport::statusTextFileReadyToImport()
{
    return tr("File is ready to import");
}
