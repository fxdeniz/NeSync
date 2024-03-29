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
#include <QtConcurrent>
#include <QJsonDocument>
#include <QStandardPaths>
#include <QTemporaryFile>

DialogImport::DialogImport(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogImport)
{
    ui->setupUi(this);
    itemDelegateAction = new TreeModelDialogImport::ItemDelegateAction(this);

    QObject::connect(this, &DialogImport::signalProgressUpdate,
                     ui->progressBar, &QProgressBar::setValue);

    QObject::connect(this, &DialogImport::signalFileImportStarted, this, [=](const QString &symbolFilePath){
        auto treeModel = (TreeModelDialogImport::Model *) ui->treeView->model();
        treeModel->markFileAsPending(symbolFilePath);
    });

    QObject::connect(this, &DialogImport::signalFileImported, this, [=](const QString &symbolFilePath){
        auto treeModel = (TreeModelDialogImport::Model *) ui->treeView->model();
        treeModel->markFileAsSuccessful(symbolFilePath);
    });

    QObject::connect(this, &DialogImport::signalFileImportFailed, this, [=](const QString &symbolFilePath){
        auto treeModel = (TreeModelDialogImport::Model *) ui->treeView->model();
        treeModel->markFileAsFailed(symbolFilePath);
    });

    QObject::connect(&futureWatcher, &QFutureWatcher<void>::finished, this, [=]{
        ui->buttonImport->hide();
        ui->buttonClearResults->show();

        if(allFilesImportedSuccessfully)
            showStatusSuccess(statusTextFileImportFinishedWithoutError(), ui->labelStatus);
        else
            showStatusError(statusTextFileImportFinishedWithError(), ui->labelStatus);
    });
}

DialogImport::~DialogImport()
{
    delete ui;
}

void DialogImport::show()
{
    if(ui->treeView->model() != nullptr)
        delete ui->treeView->model();

    showStatusInfo(statusTextWaitingForZipFile(), ui->labelStatus);
    ui->lineEdit->clear();
    ui->buttonSelectFile->setEnabled(true);
    ui->buttonImport->setDisabled(true);
    ui->buttonImport->show();
    ui->buttonClearResults->hide();
    ui->progressBar->hide();
    allFilesImportedSuccessfully = true;
    QWidget::show();
}

void DialogImport::closeEvent(QCloseEvent *event)
{
    emit accepted();
    QDialog::closeEvent(event);
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

    showStatusSuccess(statusTextZipFileReadyToImport(), ui->labelStatus);
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
    ui->treeView->hideColumn(TreeModelDialogImport::Model::ColumnIndexResult);

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
    ui->buttonSelectFile->setDisabled(true);
    ui->progressBar->setVisible(true);
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(treeModel->getTotalFileCount());
    ui->treeView->showColumn(TreeModelDialogImport::Model::ColumnIndexResult);
    showStatusInfo(statusTextFilesBeingImported(), ui->labelStatus);

    QFuture<void> future = QtConcurrent::run([=]{
        QMapIterator<QString, TreeModelDialogImport::TreeItem *> mapIterator(treeModel->getFolderItemMap());
        auto fsm = FileStorageManager::instance();
        QuaZip archive(ui->lineEdit->text());
        archive.open(QuaZip::Mode::mdUnzip);
        int progressValue = 0;

        while(mapIterator.hasNext())
        {
            mapIterator.next();

            TreeModelDialogImport::TreeItem *folderItem = mapIterator.value();
            if(folderItem->getAction() == TreeModelDialogImport::TreeItem::DoNotImport)
            {
                progressValue += folderItem->childCount();
                emit signalProgressUpdate(progressValue);
                continue;
            }

            fsm->addNewFolder(mapIterator.key(), "");

            for(int index = 0; index < folderItem->childCount(); index++)
            {
                emit signalProgressUpdate(++progressValue);

                bool addingFirstVersion = true;
                TreeModelDialogImport::TreeItem *childFileItem = folderItem->child(index);
                if(childFileItem->getAction() != TreeModelDialogImport::TreeItem::Action::Import &&
                   childFileItem->getAction() != TreeModelDialogImport::TreeItem::Action::Overwrite)
                {
                    continue;
                }

                QString symbolFilePath = childFileItem->getFileJson()[JsonKeys::File::SymbolFilePath].toString();
                QJsonObject previousFile = fsm->getFileJsonBySymbolPath(symbolFilePath);
                fsm->deleteFile(symbolFilePath);

                if(previousFile[JsonKeys::IsExist].toBool() && !previousFile[JsonKeys::File::IsFrozen].toBool())
                    emit signalFileImportStartedForActiveFile(previousFile[JsonKeys::File::UserFilePath].toString());

                emit signalFileImportStarted(symbolFilePath);

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

                    bool isAdded = false;
                    QString description = versionJson[JsonKeys::FileVersion::Description].toString();

                    if(!addingFirstVersion)
                        isAdded = fsm->appendVersion(symbolFilePath, tempFile.fileName(), description);
                    else
                    {
                        isAdded = fsm->addNewFile(childFileItem->getFileJson()[JsonKeys::File::SymbolFolderPath].toString(),
                                                  tempFile.fileName(),
                                                  true,
                                                  childFileItem->getName(),
                                                  description);
                    }

                    addingFirstVersion = false;

                    if(isAdded)
                        emit signalFileImported(symbolFilePath);
                    else
                    {
                        emit signalFileImportFailed(symbolFilePath);
                        allFilesImportedSuccessfully = false;
                    }
                }
            }
        }
    });

    futureWatcher.setFuture(future);
    ui->buttonImport->setEnabled(false);
}

void DialogImport::on_buttonClearResults_clicked()
{
    if(ui->treeView->model() != nullptr)
        delete ui->treeView->model();

    ui->lineEdit->clear();
    ui->buttonClearResults->hide();
    ui->buttonImport->show();
    ui->buttonSelectFile->setEnabled(true);
    ui->progressBar->hide();

    showStatusInfo(statusTextWaitingForZipFile(), ui->labelStatus);
}

QString DialogImport::statusTextWaitingForZipFile()
{
    return tr("Please select a <b>zip file</b>");
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

QString DialogImport::statusTextZipFileReadyToImport()
{
    return tr("<b>Zip file</b> is ready to import. (<b>NOTE:</b> All files you import will be imported as frozen)");
}

QString DialogImport::statusTextFilesBeingImported()
{
    return tr("Files being imported in background...");
}

QString DialogImport::statusTextFileImportFinishedWithoutError()
{
    return tr("<b>All files</b> added successfully");
}

QString DialogImport::statusTextFileImportFinishedWithError()
{
    return tr("Not all files added successfully, check the results for details");
}
