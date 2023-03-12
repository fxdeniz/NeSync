#include "DialogImport.h"
#include "ui_DialogImport.h"
#include "DataModels/DialogImport/TreeModelDialogImport.h"

#include <quazip/quazip.h>
#include <quazip/quazipfile.h>
#include <QFileDialog>
#include <QJsonDocument>
#include <QStandardPaths>

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

    showStatusSuccess(statusTextFileReadyToImport(), ui->labelStatus);
    ui->buttonImport->setEnabled(true);

    if(ui->treeView->model() != nullptr)
        delete ui->treeView->model();

    auto treeModel = new TreeModelDialogImport::Model(document.array(), ui->treeView);
    ui->treeView->setModel(treeModel);

    ui->treeView->header()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
    ui->treeView->header()->setSectionResizeMode(TreeModelDialogImport::Model::ColumnIndexSymbolPath, QHeaderView::ResizeMode::Interactive);
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
