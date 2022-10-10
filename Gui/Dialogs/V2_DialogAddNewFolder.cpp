#include "V2_DialogAddNewFolder.h"
#include "ui_V2_DialogAddNewFolder.h"

#include "Tasks/TaskAddNewFolders.h"
#include "Backend/FileStorageSubSystem/FileStorageManager.h"

#include <QFileIconProvider>
#include <QStandardPaths>
#include <QHashIterator>
#include <QStorageInfo>
#include <QFileDialog>

V2_DialogAddNewFolder::V2_DialogAddNewFolder(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::V2_DialogAddNewFolder)
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

V2_DialogAddNewFolder::~V2_DialogAddNewFolder()
{
    delete ui;
}

void V2_DialogAddNewFolder::show(const QString &_parentFolderPath)
{
    this->parentFolderPath = _parentFolderPath;
    ui->labelParentFolderPath->setText(_parentFolderPath);

    showStatusInfo(statusTextWaitingForFolder(), ui->labelStatus);
    if(ui->lineEditFolderPath->text().isEmpty())
        ui->labelFolderName->setText(tr("New Folder Name"));

    ui->lineEditFolderPath->setFocus();
    ui->lineEditFolderPath->selectedText();

    QWidget::show();
}

void V2_DialogAddNewFolder::closeEvent(QCloseEvent *event)
{
    emit accepted();
    QDialog::closeEvent(event);
}

void V2_DialogAddNewFolder::on_buttonSelectFolder_clicked()
{
    QFileDialog dialog(this);
    dialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::StandardLocation::DesktopLocation));
    dialog.setFileMode(QFileDialog::FileMode::Directory);

    if(dialog.exec())
    {
        auto fsm = FileStorageManager::instance();

        QStorageInfo storageInfo(fsm->getBackupDirectory());
        qint64 folderSize = getFolderSize(dialog.selectedFiles().at(0));
        qint64 availableSize = storageInfo.bytesFree();

        if(folderSize > availableSize)
        {
            showStatusWarning(statusTextNoFreeSpace(dialog.selectedFiles().at(0)), ui->labelStatus);
            return;
        }

        bool isFolderExistByUserPath = fsm->isFolderExistByUserFolderPath(dialog.selectedFiles().at(0));

        if(isFolderExistByUserPath)
        {
            this->showStatusWarning(statusTextFolderExist(dialog.selectedFiles().at(0)), ui->labelStatus);
            return;
        }

        QObject::connect(model, &QFileSystemModel::directoryLoaded, model,
                         [=](const QString path){

            if(path == model->rootPath()) // Reject clicks to treeView items
            {
                ui->buttonAddFilesToDb->setEnabled(true);
                showStatusInfo(statusTextContentReadyToAdd(), ui->labelStatus);
                ui->treeView->resizeColumnToContents(CustomFileSystemModel::ColumnIndex::Name);
                ui->treeView->resizeColumnToContents(CustomFileSystemModel::ColumnIndex::AutoSync);
            }
        });

        ui->lineEditFolderPath->setText(dialog.selectedFiles().at(0));
        QModelIndex rootIndex = model->setRootPath(ui->lineEditFolderPath->text());        
        ui->treeView->setRootIndex(rootIndex);

        QDir selectedUserDir(ui->lineEditFolderPath->text());
        ui->labelFolderName->setText(selectedUserDir.dirName());
    }
}


void V2_DialogAddNewFolder::on_treeView_doubleClicked(const QModelIndex &index)
{
    if(ui->buttonAddFilesToDb->isEnabled())
        model->updateAutoSyncStatusOfItem(index);
}

QMap<QString, V2_DialogAddNewFolder::FolderItem> V2_DialogAddNewFolder::createBufferWithFolderOnly()
{
    QMap<QString, FolderItem> result;

    QString parentSymbolDir =  ui->labelParentFolderPath->text() + ui->labelFolderName->text();
    QDir rootDir = model->rootDirectory();
    rootDir.setFilter(QDir::Filter::Dirs | QDir::Filter::NoDotAndDotDot);
    QDirIterator cursor(rootDir, QDirIterator::IteratorFlag::Subdirectories);

    FolderItem firstItem;
    firstItem.userDir = QDir::toNativeSeparators(model->rootPath() + QDir::separator());
    firstItem.symbolDir = parentSymbolDir + FileStorageManager::CONST_SYMBOL_DIRECTORY_SEPARATOR;
    result.insert(model->rootPath(), firstItem);

    while(cursor.hasNext())
    {
        FolderItem item;
        QString currentUserDir = cursor.next();

        item.userDir = QDir::toNativeSeparators(currentUserDir + QDir::separator());
        item.symbolDir = generateSymbolDirFrom(currentUserDir, model->rootPath(), parentSymbolDir);
        result.insert(currentUserDir, item);
    }
    return result;
}

void V2_DialogAddNewFolder::addFilesToBuffer(QMap<QString, FolderItem> &buffer)
{
    QDirIterator cursor(model->rootPath(), QDir::Filter::Files, QDirIterator::IteratorFlag::Subdirectories);

    while(cursor.hasNext())
    {
        QFileInfo info = cursor.nextFileInfo();
        FolderItem item = buffer.value(info.absolutePath());
        bool isEnabled = model->isAutoSyncEnabledFor(info.filePath());
        item.files.insert(info.filePath(), isEnabled);
        buffer.insert(info.absolutePath(), item);
    }
}

QString V2_DialogAddNewFolder::generateSymbolDirFrom(const QString &userDir, const QString &parentUserDir, const QString &parentSymbolDir)
{
    QString parentDir = QDir::toNativeSeparators(parentUserDir);
    QString currentUserDir = QDir::toNativeSeparators(userDir + QDir::separator());

    auto suffix = currentUserDir.split(parentDir).last();
    suffix = QDir::toNativeSeparators(suffix);
    suffix.prepend(parentSymbolDir);
    suffix.replace(QDir::separator(), FileStorageManager::CONST_SYMBOL_DIRECTORY_SEPARATOR);

    return suffix;
}

QString V2_DialogAddNewFolder::statusTextWaitingForFolder()
{
    return tr("Please select a folder");
}

QString V2_DialogAddNewFolder::statusTextContentReadyToAdd()
{
    QString text = tr("<b>All folder</b> content ready to add");
    return text;
}

QString V2_DialogAddNewFolder::statusTextEmptyFolder()
{
    return tr("Folder name cannot be <b>empty</b>");
}

QString V2_DialogAddNewFolder::statusTextAdding()
{
    return tr("Folders & files are being added in background...");
}

QString V2_DialogAddNewFolder::statusTextNoFreeSpace(QString folderName)
{
    QString text = tr("Not enough free space available for: <b>%1</b>");
    text = text.arg(folderName);
    return text;
}

QString V2_DialogAddNewFolder::statusTextFolderExist(QString folderName)
{
    QString text = tr("Folder already added: <b>%1</b>");
    text = text.arg(folderName);
    return text;
}

QString V2_DialogAddNewFolder::statusTextSuccess(QString folderName)
{
    QString text = tr("Folder <b>%1</b> created successfully");
    text = text.arg(folderName);
    return text;
}

QString V2_DialogAddNewFolder::statusTextError(QString folderName)
{
    QString text = tr("Error ocured while creating folder <b>%1</b>");
    text = text.arg(folderName);
    return text;
}

void V2_DialogAddNewFolder::on_buttonAddFilesToDb_clicked()
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
                     this, &V2_DialogAddNewFolder::refreshTreeView);

    QObject::connect(task, &TaskAddNewFolders::signalFileBeingProcessed,
                     model, &CustomFileSystemModel::markItemAsPending);

    QObject::connect(task, &TaskAddNewFolders::signalFileAddedSuccessfully,
                     model, &CustomFileSystemModel::markItemAsSuccessful);

    QObject::connect(task, &TaskAddNewFolders::signalFileAddingFailed,
                     model, &CustomFileSystemModel::markItemAsFailed);

    QObject::connect(task, &TaskAddNewFolders::signalFileProcessed,
                     this->ui->progressBar, &QProgressBar::setValue);

    QObject::connect(task, &TaskAddNewFolders::finished,
                     this, &V2_DialogAddNewFolder::slotOnTaskAddNewFoldersFinished);

    QObject::connect(task, &QThread::finished,
                     task, &QThread::deleteLater);

    task->start();
}

void V2_DialogAddNewFolder::slotOnTaskAddNewFoldersFinished(bool isAllRequestSuccessful)
{
    if(isAllRequestSuccessful)
        this->showStatusSuccess("<b>All folder</b> content added successfully", ui->labelStatus);
    else
        this->showStatusError("Not all files added successfully, check the results for details", ui->labelStatus);

    ui->buttonAddFilesToDb->hide();
    ui->buttonClearResults->show();
}

void V2_DialogAddNewFolder::refreshTreeView()
{
    ui->treeView->viewport()->update();
    ui->treeView->resizeColumnToContents(CustomFileSystemModel::ColumnIndex::Name);
    ui->treeView->resizeColumnToContents(CustomFileSystemModel::ColumnIndex::AutoSync);
    ui->treeView->resizeColumnToContents(CustomFileSystemModel::ColumnIndex::Status);
}

void V2_DialogAddNewFolder::on_buttonClearResults_clicked()
{
    ui->labelFolderName->setText("New Folder Name");
    ui->progressBar->setValue(0);
    ui->progressBar->hide();
    ui->lineEditFolderPath->clear();
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

qint64 V2_DialogAddNewFolder::getFolderSize(const QString &pathToFolder)
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

