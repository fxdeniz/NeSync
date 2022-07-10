#include "DialogAddNewFile.h"
#include "ui_DialogAddNewFile.h"

#include <QFileIconProvider>
#include <QStandardPaths>
#include <QStorageInfo>
#include <QtConcurrent>
#include <QFileDialog>
#include <QFileInfo>

#include "Backend/FileStorageSubSystem/FileStorageManager.h"
#include "Tasks/TaskAddNewFiles.h"

DialogAddNewFile::DialogAddNewFile(const QString &targetFolder, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAddNewFile)
{
    ui->setupUi(this);
    this->ui->clbAddNewFiles->setVisible(false);

    this->targetSymbolFolder = targetFolder;

    this->ui->labelTargetFolder->setText(this->targetSymbolFolder);
    this->ui->progressBar->setVisible(false);
    this->ui->clbAddFilesToDb->setEnabled(false);
    this->ui->buttonRemoveFile->setEnabled(false);

    QFileIconProvider iconProvider;
    auto pixmap = iconProvider.icon(QFileIconProvider::IconType::Folder).pixmap(24, 24);
    this->ui->labelTargetFolderIcon->setPixmap(pixmap);
    this->ui->labelTargetFolderIcon->setMask(pixmap.mask());

    QList<TableModelNewAddedFiles::TableItem> sampleFileExplorerTableData;

    this->tableModelNewAddedFiles = new TableModelNewAddedFiles(sampleFileExplorerTableData, this);
    this->ui->tableView->setModel(this->tableModelNewAddedFiles);
    this->showStatusInfo("Please select files from your local file system");

    this->comboBoxDelegateAutoSync = new ComboBoxItemDelegateAutoSync(this);
    this->ui->tableView->setItemDelegateForColumn(1, this->comboBoxDelegateAutoSync);
}

DialogAddNewFile::~DialogAddNewFile()
{
    delete ui;
}

void DialogAddNewFile::on_buttonSelectNewFile_clicked()
{
    QFileDialog dialog(this);
    dialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::StandardLocation::DesktopLocation));
    dialog.setFileMode(QFileDialog::FileMode::ExistingFile);

    if(dialog.exec())
    {
        auto selectedFilePath = dialog.selectedFiles().at(0);
        selectedFilePath = QDir::toNativeSeparators(selectedFilePath);

        QFile selectedFile(selectedFilePath);
        if(!selectedFile.exists())
        {
            this->showStatusWarning("File not found: " + selectedFilePath);
            return;
        }

        bool isFileOpened = selectedFile.open(QFile::OpenModeFlag::ReadOnly);

        if(!isFileOpened)
        {
            this->showStatusWarning("File couldn't opened: " + selectedFilePath);
            return;
        }

        auto fsm = FileStorageManager::instance();

        QStorageInfo storageInfo(fsm->getBackupDirectory());
        auto fileSize = selectedFile.size();
        auto availableSize = storageInfo.bytesFree();

        if(fileSize > availableSize)
        {
            this->showStatusWarning("Not enough free space available for: " + selectedFilePath);
            return;
        }

        QFileInfo fileInfo(selectedFile);
        auto absolutePath = QDir::toNativeSeparators(fileInfo.absolutePath()) + QDir::separator();
        TableModelNewAddedFiles::TableItem item {fileInfo.fileName(),
                                                true,
                                                TableModelNewAddedFiles::TableItemStatus::Waiting,
                                                absolutePath};

        bool isAlreadySelected = this->tableModelNewAddedFiles->getItemList().contains(item);

        if(isAlreadySelected)
        {
            this->showStatusWarning("File already selected: " + selectedFilePath);
            return;
        }

        bool isAlreadyAddedToDb = fsm->isFileExistByUserFilePath(selectedFilePath);

        if(isAlreadyAddedToDb)
        {
            this->showStatusWarning("File already added to database: " + selectedFilePath);
            return;
        }

        auto tableModel = this->tableModelNewAddedFiles;
        tableModel->insertRows(0, 1, QModelIndex());

        QModelIndex index = tableModel->index(0, 0, QModelIndex());
        tableModel->setData(index, item.fileName, Qt::EditRole);

        index = tableModel->index(0, 1, QModelIndex());
        tableModel->setData(index, item.isAutoSyncEnabled, Qt::EditRole);

        index = tableModel->index(0, 2, QModelIndex());
        tableModel->setData(index, item.location, Qt::EditRole);

        this->ui->tableView->openPersistentEditor(this->tableModelNewAddedFiles->index(index.row(), 1));

        this->ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Interactive);
        this->ui->tableView->resizeColumnsToContents();
        this->showStatusNormal(""); // Clean status message
    }

    if(this->tableModelNewAddedFiles->getItemList().size() > 0)
    {
        this->ui->buttonRemoveFile->setEnabled(true);
        this->ui->clbAddFilesToDb->setEnabled(true);
        this->ui->progressBar->setVisible(false);
    }
}

void DialogAddNewFile::showStatusNormal(const QString &message)
{
    auto labelStatus = this->ui->labelStatus;

    QPalette palette;
    palette.setColor(QPalette::ColorRole::WindowText, Qt::GlobalColor::black);
    labelStatus->setPalette(palette);

    labelStatus->setPalette(palette);
    labelStatus->setAutoFillBackground(true);
    labelStatus->setText(message);
}

void DialogAddNewFile::showStatusInfo(const QString &message)
{
    auto labelStatus = this->ui->labelStatus;

    QPalette palette;
    palette.setColor(QPalette::ColorRole::Window, "#7ed6df");
    palette.setColor(QPalette::ColorRole::WindowText, Qt::GlobalColor::black);

    labelStatus->setPalette(palette);
    labelStatus->setAutoFillBackground(true);
    labelStatus->setText(message);
}

void DialogAddNewFile::showStatusWarning(const QString &message)
{
    auto labelStatus = this->ui->labelStatus;

    QPalette palette;
    palette.setColor(QPalette::ColorRole::Window, "#f6e58d");
    palette.setColor(QPalette::ColorRole::WindowText, Qt::GlobalColor::black);

    labelStatus->setPalette(palette);
    labelStatus->setAutoFillBackground(true);
    labelStatus->setText(message);
}

void DialogAddNewFile::showStatusError(const QString &message)
{
    auto labelStatus = this->ui->labelStatus;

    QPalette palette;
    palette.setColor(QPalette::ColorRole::Window, "#ff3838");
    palette.setColor(QPalette::ColorRole::WindowText, Qt::GlobalColor::black);

    labelStatus->setPalette(palette);
    labelStatus->setAutoFillBackground(true);
    labelStatus->setText(message);
}

void DialogAddNewFile::showStatusSuccess(const QString &message)
{
    auto labelStatus = this->ui->labelStatus;

    QPalette palette;
    palette.setColor(QPalette::ColorRole::Window, "#b8e994");
    palette.setColor(QPalette::ColorRole::WindowText, Qt::GlobalColor::black);

    labelStatus->setPalette(palette);
    labelStatus->setAutoFillBackground(true);
    labelStatus->setText(message);
}


void DialogAddNewFile::on_buttonRemoveFile_clicked()
{
    QItemSelectionModel *selectionModel = this->ui->tableView->selectionModel();
    const QModelIndexList indices = selectionModel->selectedRows();

    // Start deleting from last item to first item.
    for(int i = indices.count() - 1; i >= 0; i--)
    {
        QModelIndex index = indices.at(i);
        int row = index.row();
        this->tableModelNewAddedFiles->removeRows(row, 1);
    }

    if(!indices.isEmpty())
        this->showStatusNormal(""); // Clean status message

    if(this->tableModelNewAddedFiles->getItemList().size() > 0)
        this->ui->clbAddFilesToDb->setEnabled(true);
    else
    {
        this->ui->buttonRemoveFile->setEnabled(false);
        this->ui->clbAddFilesToDb->setEnabled(false);
        this->ui->progressBar->setVisible(false);
    }
}


void DialogAddNewFile::on_clbAddFilesToDb_clicked()
{
    this->ui->progressBar->setVisible(true);
    this->ui->buttonSelectNewFile->setEnabled(false);
    this->ui->buttonRemoveFile->setEnabled(false);
    this->ui->clbAddFilesToDb->setEnabled(false);
    emit signalDisableDelegatesOfAutoSyncColumn(true);
    this->showStatusInfo("Files are being added in background...");

    TaskAddNewFiles *task = new TaskAddNewFiles(this->targetSymbolFolder, this);

    for(const TableModelNewAddedFiles::TableItem &item : this->tableModelNewAddedFiles->getItemList())
    {
        auto pathToFile = item.location + item.fileName;
        if(item.isAutoSyncEnabled)
            task->addAutoSyncEnabled(pathToFile);
        else
            task->addAutoSyncDisabled(pathToFile);
    }

    this->ui->progressBar->setMaximum(task->fileCount());

    QObject::connect(task, &TaskAddNewFiles::signalFileBeingProcessed,
                     this->tableModelNewAddedFiles, &TableModelNewAddedFiles::markItemAsPending);

    QObject::connect(task, &TaskAddNewFiles::signalFileBeingProcessed,
                     this, &DialogAddNewFile::refreshTableView);

    QObject::connect(task, &TaskAddNewFiles::signalFileAddedSuccessfully,
                     this->tableModelNewAddedFiles, &TableModelNewAddedFiles::markItemAsSuccessful);

    QObject::connect(task, &TaskAddNewFiles::signalFileAddingFailed,
                     this->tableModelNewAddedFiles, &TableModelNewAddedFiles::markItemAsFailed);

    QObject::connect(task, &TaskAddNewFiles::signalFileProcessed,
                     this->ui->progressBar, &QProgressBar::setValue);

    QObject::connect(task, &TaskAddNewFiles::finished,
                     this, &DialogAddNewFile::onTaskAddNewFilesFinished);

    QObject::connect(task, &QThread::finished,
                     task, &QThread::deleteLater);

    task->start();
}

void DialogAddNewFile::on_clbAddNewFiles_clicked()
{
    this->tableModelNewAddedFiles->removeRows(0, this->ui->tableView->model()->rowCount());

    this->ui->clbAddNewFiles->setVisible(false);

    this->ui->buttonSelectNewFile->setVisible(true);
    this->ui->buttonSelectNewFile->setEnabled(true);

    this->ui->buttonRemoveFile->setVisible(true);
    this->ui->buttonRemoveFile->setEnabled(false);

    this->ui->clbAddFilesToDb->setVisible(true);
    this->ui->clbAddFilesToDb->setEnabled(false);

    this->ui->progressBar->reset();
    this->showStatusInfo("Please select files from your local file system");
}

void DialogAddNewFile::onTaskAddNewFilesFinished(bool isAllRequestSuccessful)
{
    if(isAllRequestSuccessful)
        this->showStatusSuccess("All files added successfully");
    else
        this->showStatusError("Not all files added successfully, check the results for details");

    this->ui->clbAddFilesToDb->setVisible(false);
    this->ui->buttonSelectNewFile->setVisible(false);
    this->ui->buttonRemoveFile->setVisible(false);

    this->ui->clbAddNewFiles->setVisible(true);
}

void DialogAddNewFile::refreshTableView(const QString &dummy)
{
    Q_UNUSED(dummy);

    this->ui->tableView->viewport()->update();
}

