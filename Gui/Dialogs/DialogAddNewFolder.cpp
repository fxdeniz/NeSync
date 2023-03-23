#include "DialogAddNewFolder.h"
#include "ui_DialogAddNewFolder.h"

#include "Utility/JsonDtoFormat.h"
#include "Tasks/TaskAddNewFolders.h"
#include "Backend/FileStorageSubSystem/FileStorageManager.h"

#include <QFileIconProvider>
#include <QStandardPaths>
#include <QHashIterator>
#include <QStorageInfo>
#include <QFileDialog>

DialogAddNewFolder::DialogAddNewFolder(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAddNewFolder)
{
    ui->setupUi(this);

    QFileIconProvider iconProvider;
    ui->buttonClearResults->hide();
    ui->progressBar->hide();
    ui->buttonAddFilesToDb->setEnabled(false);

    auto pixmap = iconProvider.icon(QFileIconProvider::IconType::Folder).pixmap(24, 24);
    ui->labelFolderIcon->setPixmap(pixmap);
    ui->labelFolderIcon->setMask(pixmap.mask());

    model = new CustomFileSystemModel(this);
    //model->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
    ui->treeView->setModel(model);
    ui->treeView->header()->setStretchLastSection(false);
    ui->treeView->header()->setMinimumSectionSize(200);
    ui->treeView->header()->setSectionResizeMode(QHeaderView::ResizeMode::Interactive);
    ui->treeView->hideColumn(CustomFileSystemModel::ColumnIndex::Size);
    ui->treeView->hideColumn(CustomFileSystemModel::ColumnIndex::Type);
    ui->treeView->hideColumn(CustomFileSystemModel::ColumnIndex::DateModified);
    ui->treeView->hideColumn(CustomFileSystemModel::ColumnIndex::Status);
}

DialogAddNewFolder::~DialogAddNewFolder()
{
    delete ui;
}

void DialogAddNewFolder::show(const QString &parentFolderPath, FileMonitoringManager *fmm)
{
    this->parentFolderPath = parentFolderPath;
    this->fmm = fmm;
    ui->labelParentFolderPath->setText(parentFolderPath);

    showStatusInfo(statusTextWaitingForFolder(), ui->labelStatus);
    if(ui->lineEdit->text().isEmpty())
        ui->labelFolderName->setText(tr("New Folder Name"));

    ui->lineEdit->setFocus();
    ui->lineEdit->selectedText();

    QWidget::show();
}

void DialogAddNewFolder::closeEvent(QCloseEvent *event)
{
    emit accepted();
    QDialog::closeEvent(event);
}

void DialogAddNewFolder::on_buttonSelectFolder_clicked()
{
    QFileDialog dialog(this);
    dialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::StandardLocation::DesktopLocation));
    dialog.setFileMode(QFileDialog::FileMode::Directory);

    if(dialog.exec())
    {
        auto fsm = FileStorageManager::instance();
        QString selectedFolderPath = QDir::toNativeSeparators(dialog.selectedFiles().at(0)).append(QDir::separator());

        QStorageInfo storageInfo(fsm->getBackupFolderPath());
        qint64 folderSize = getFolderSize(selectedFolderPath);
        qint64 availableSize = storageInfo.bytesFree();

        if(folderSize > availableSize)
        {
            showStatusWarning(statusTextNoFreeSpace(selectedFolderPath), ui->labelStatus);
            return;
        }

        QJsonObject folderJson = fsm->getFolderJsonByUserPath(selectedFolderPath);
        bool isFolderExistByUserPath = folderJson[JsonKeys::IsExist].toBool();

        if(isFolderExistByUserPath)
        {
            this->showStatusWarning(statusTextFolderExist(selectedFolderPath), ui->labelStatus);
            return;
        }

        QObject::connect(model, &QFileSystemModel::directoryLoaded, model,
                         [=](const QString path){

            if(path == model->rootPath()) // Reject clicks to treeView items
            {
                ui->buttonAddFilesToDb->setEnabled(true);
                showStatusInfo(statusTextContentReadyToAdd(), ui->labelStatus);
                ui->treeView->resizeColumnToContents(CustomFileSystemModel::ColumnIndex::Name);
                ui->treeView->resizeColumnToContents(CustomFileSystemModel::ColumnIndex::Frozen);
            }
        });

        ui->lineEdit->setText(selectedFolderPath);
        QModelIndex rootIndex = model->setRootPath(ui->lineEdit->text());
        ui->treeView->setRootIndex(rootIndex);

        QDir selectedUserDir(ui->lineEdit->text());
        ui->labelFolderName->setText(selectedUserDir.dirName());
    }
}


void DialogAddNewFolder::on_treeView_doubleClicked(const QModelIndex &index)
{
    if(ui->buttonAddFilesToDb->isEnabled())
        model->updateFrozenStatusOfItem(index);
}

QMap<QString, DialogAddNewFolder::FolderItem> DialogAddNewFolder::createBufferWithFolderOnly()
{
    QMap<QString, FolderItem> result;

    QString parentSymbolFolder =  ui->labelParentFolderPath->text() + ui->labelFolderName->text();
    QDir rootFolder = model->rootDirectory();
    rootFolder.setFilter(QDir::Filter::Dirs | QDir::Filter::NoDotAndDotDot);
    QDirIterator cursor(rootFolder, QDirIterator::IteratorFlag::Subdirectories);

    FolderItem firstItem;
    firstItem.userFolderPath = QDir::toNativeSeparators(model->rootPath() + QDir::separator());
    firstItem.symbolFolderPath = parentSymbolFolder + FileStorageManager::separator;
    result.insert(model->rootPath(), firstItem);

    while(cursor.hasNext())
    {
        FolderItem item;
        QString currentUserDir = cursor.next();

        item.userFolderPath = QDir::toNativeSeparators(currentUserDir + QDir::separator());
        item.symbolFolderPath = generateSymbolFolderPathFrom(currentUserDir, model->rootPath(), parentSymbolFolder);
        result.insert(currentUserDir, item);
    }
    return result;
}

void DialogAddNewFolder::addFilesToBuffer(QMap<QString, FolderItem> &buffer)
{
    QDirIterator cursor(model->rootPath(), QDir::Filter::Files, QDirIterator::IteratorFlag::Subdirectories);

    while(cursor.hasNext())
    {
        QFileInfo info = cursor.nextFileInfo();
        FolderItem item = buffer.value(info.absolutePath());
        bool isFrozen = model->isFileMarkedAsFrozen(info.filePath());
        item.files.insert(info.filePath(), isFrozen);
        buffer.insert(info.absolutePath(), item);
    }
}

QString DialogAddNewFolder::generateSymbolFolderPathFrom(const QString &userFolderPath,
                                                         const QString &parentUserFolderPath,
                                                         const QString &parentSymbolFolderPath)
{
    QString parentFolder = QDir::toNativeSeparators(parentUserFolderPath);
    QString currentUserFolder = QDir::toNativeSeparators(userFolderPath + QDir::separator());

    auto suffix = currentUserFolder.split(parentFolder).last();
    suffix = QDir::toNativeSeparators(suffix);
    suffix.prepend(parentSymbolFolderPath);
    suffix.replace(QDir::separator(), FileStorageManager::separator);

    return suffix;
}

QString DialogAddNewFolder::statusTextWaitingForFolder()
{
    return tr("Please select a folder");
}

QString DialogAddNewFolder::statusTextContentReadyToAdd()
{
    QString text = tr("<b>All folder</b> content ready to add");
    return text;
}

QString DialogAddNewFolder::statusTextEmptyFolder()
{
    return tr("Folder name cannot be <b>empty</b>");
}

QString DialogAddNewFolder::statusTextAdding()
{
    return tr("Folders & files are being added in background...");
}

QString DialogAddNewFolder::statusTextNoFreeSpace(QString folderName)
{
    QString text = tr("Not enough free space available for: <b>%1</b>");
    text = text.arg(folderName);
    return text;
}

QString DialogAddNewFolder::statusTextFolderExist(QString folderName)
{
    QString text = tr("Folder already added: <b>%1</b>");
    text = text.arg(folderName);
    return text;
}

QString DialogAddNewFolder::statusTextSuccess(QString folderName)
{
    QString text = tr("Folder <b>%1</b> created successfully");
    text = text.arg(folderName);
    return text;
}

QString DialogAddNewFolder::statusTextError(QString folderName)
{
    QString text = tr("Error ocured while creating folder <b>%1</b>");
    text = text.arg(folderName);
    return text;
}

void DialogAddNewFolder::on_buttonAddFilesToDb_clicked()
{
    ui->buttonSelectFolder->setEnabled(false);
    ui->buttonAddFilesToDb->setEnabled(false);
    ui->treeView->showColumn(CustomFileSystemModel::ColumnIndex::Status);
    this->showStatusInfo(statusTextAdding(), ui->labelStatus);
    ui->progressBar->show();
    ui->treeView->expandAll();

    auto buffer = createBufferWithFolderOnly();
    addFilesToBuffer(buffer);
    QList<FolderItem> result = buffer.values();

    TaskAddNewFolders *task = new TaskAddNewFolders(result, this);

    int fileCount = task->fileCount();
    ui->progressBar->setMaximum(fileCount);

    if(fileCount <= 0) // Prevent infinite progressbar when have only folders
        ui->progressBar->hide();

    QObject::connect(task, &TaskAddNewFolders::signalGenericFileEvent,
                     this, &DialogAddNewFolder::refreshTreeView);

    QObject::connect(task, &TaskAddNewFolders::signalFolderAdded,
                     fmm, &FileMonitoringManager::addTargetAtRuntime,
                     Qt::ConnectionType::BlockingQueuedConnection);

    QObject::connect(task, &TaskAddNewFolders::signalFileAddedSuccessfully,
                     fmm, &FileMonitoringManager::addTargetAtRuntime,
                     Qt::ConnectionType::BlockingQueuedConnection);

    QObject::connect(task, &QThread::started,
                     fmm, &FileMonitoringManager::pauseMonitoring,
                     Qt::ConnectionType::BlockingQueuedConnection);

    QObject::connect(task, &QThread::finished,
                     fmm, &FileMonitoringManager::continueMonitoring,
                     Qt::ConnectionType::BlockingQueuedConnection);

    QObject::connect(task, &TaskAddNewFolders::signalFileBeingProcessed,
                     model, &CustomFileSystemModel::markItemAsPending);

    QObject::connect(task, &TaskAddNewFolders::signalFileAddedSuccessfully,
                     model, &CustomFileSystemModel::markItemAsSuccessful);

    QObject::connect(task, &TaskAddNewFolders::signalFileAddingFailed,
                     model, &CustomFileSystemModel::markItemAsFailed);

    QObject::connect(task, &TaskAddNewFolders::signalFileProcessed,
                     this->ui->progressBar, &QProgressBar::setValue);

    QObject::connect(task, &TaskAddNewFolders::finished,
                     this, &DialogAddNewFolder::slotOnTaskAddNewFoldersFinished);

    QObject::connect(task, &QThread::finished,
                     task, &QThread::deleteLater);

    task->start();
}

void DialogAddNewFolder::on_buttonClearResults_clicked()
{
    ui->labelFolderName->setText("New Folder Name");
    ui->progressBar->setValue(0);
    ui->progressBar->hide();
    ui->lineEdit->clear();
    ui->buttonSelectFolder->setEnabled(true);
    ui->buttonSelectFolder->show();
    ui->buttonAddFilesToDb->show();
    ui->buttonClearResults->hide();
    showStatusInfo(statusTextWaitingForFolder(), ui->labelStatus);

    ui->treeView->hideColumn(CustomFileSystemModel::ColumnIndex::Status);

    CustomFileSystemModel *ptr = new CustomFileSystemModel(this);
    ui->treeView->setModel(ptr);
    delete model;
    model = ptr;
}

void DialogAddNewFolder::slotOnTaskAddNewFoldersFinished(bool isAllRequestSuccessful)
{
    if(isAllRequestSuccessful)
        this->showStatusSuccess("<b>All folder</b> content added successfully", ui->labelStatus);
    else
        this->showStatusError("Not all files added successfully, check the results for details", ui->labelStatus);

    ui->buttonAddFilesToDb->hide();
    ui->buttonClearResults->show();
}

void DialogAddNewFolder::refreshTreeView()
{
    ui->treeView->viewport()->update();
    ui->treeView->resizeColumnToContents(CustomFileSystemModel::ColumnIndex::Name);
    ui->treeView->resizeColumnToContents(CustomFileSystemModel::ColumnIndex::Frozen);
    ui->treeView->resizeColumnToContents(CustomFileSystemModel::ColumnIndex::Status);
}

qint64 DialogAddNewFolder::getFolderSize(const QString &pathToFolder)
{
    qint64 result = 0;
    QDirIterator cursor(pathToFolder,
                        QDir::Filter::Files | QDir::Filter::NoDotAndDotDot,
                        QDirIterator::IteratorFlag::Subdirectories);

    while(cursor.hasNext())
    {
        result += cursor.nextFileInfo().size();
    }

    return result;
}

