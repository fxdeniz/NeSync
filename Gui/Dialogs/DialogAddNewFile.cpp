#include "DialogAddNewFile.h"
#include "ui_DialogAddNewFile.h"

#include <QFileIconProvider>
#include <QStandardPaths>
#include <QStorageInfo>
#include <QtConcurrent>
#include <QFileDialog>
#include <QFileInfo>

DialogAddNewFile::DialogAddNewFile(FileStorageManager *fsm, const QString &targetFolder, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAddNewFile)
{
    ui->setupUi(this);

    this->targetSymbolFolder = targetFolder;

    this->ui->labelTargetFolder->setText(this->targetSymbolFolder);
    this->ui->progressBar->setVisible(false);
    this->ui->commandLinkButton->setEnabled(false);
    this->ui->buttonRemoveFile->setEnabled(false);

    QFileIconProvider iconProvider;
    auto pixmap = iconProvider.icon(QFileIconProvider::IconType::Folder).pixmap(24, 24);
    this->ui->labelTargetFolderIcon->setPixmap(pixmap);
    this->ui->labelTargetFolderIcon->setMask(pixmap.mask());

    this->fileStorageManager = fsm;

    QList<TableModelNewAddedFiles::TableItem> sampleFileExplorerTableData;

    this->tableModelNewAddedFiles = new TableModelNewAddedFiles(sampleFileExplorerTableData, this);
    this->ui->tableView->setModel(this->tableModelNewAddedFiles);
    this->showStatusInfo("Please select files from your local file system");
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

        QStorageInfo storageInfo(this->fileStorageManager->getBackupDirectory());
        auto fileSize = selectedFile.size();
        auto availableSize = storageInfo.bytesFree();

        if(fileSize > availableSize)
        {
            this->showStatusWarning("Not enough free space available for: " + selectedFilePath);
            return;
        }

        QFileInfo fileInfo(selectedFile);
        auto absolutePath = QDir::toNativeSeparators(fileInfo.absolutePath()) + QDir::separator();
        TableModelNewAddedFiles::TableItem item {fileInfo.fileName(), absolutePath};
        bool isAlreadySelected = this->tableModelNewAddedFiles->getItemList().contains(item);

        if(isAlreadySelected)
        {
            this->showStatusWarning("File already selected: " + selectedFilePath);
            return;
        }

        bool isAlreadyAddedToDb = this->fileStorageManager->isFileExistByUserFilePath(selectedFilePath);

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
        tableModel->setData(index, item.location, Qt::EditRole);

        this->ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Interactive);
        this->ui->tableView->resizeColumnsToContents();
        this->showStatusNormal(""); // Clean status message
    }

    if(this->tableModelNewAddedFiles->getItemList().size() > 0)
    {
        this->ui->buttonRemoveFile->setEnabled(true);
        this->ui->commandLinkButton->setEnabled(true);
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

bool DialogAddNewFile::postToFSM(const QString &pathToFile)
{
    QFileInfo fileInfo(pathToFile);
    QString userDirectory = QDir::toNativeSeparators(fileInfo.absolutePath()) + QDir::separator();
    bool requestResult = this->fileStorageManager->addNewFile(pathToFile,
                                                              this->targetSymbolFolder,
                                                              false,
                                                              true,
                                                              userDirectory
                                                              );

    return requestResult;
}

QScopedPointer<FileStorageManager> DialogAddNewFile::createFSM() const
{
    auto appDataDir = QStandardPaths::writableLocation(QStandardPaths::StandardLocation::TempLocation);
    appDataDir = QDir::toNativeSeparators(appDataDir);
    appDataDir += QDir::separator();

    auto backupDir = appDataDir + "backup" + QDir::separator();
    auto symbolDir = appDataDir + "symbols" + QDir::separator();
    QDir dir;
    dir.mkdir(backupDir);
    dir.mkdir(symbolDir);

    return QScopedPointer<FileStorageManager>(new FileStorageManager(backupDir, symbolDir));
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
        this->ui->commandLinkButton->setEnabled(true);
    else
    {
        this->ui->buttonRemoveFile->setEnabled(false);
        this->ui->commandLinkButton->setEnabled(false);
        this->ui->progressBar->setVisible(false);
    }
}


void DialogAddNewFile::on_commandLinkButton_clicked()
{
    this->ui->progressBar->setVisible(true);

    auto files = this->tableModelNewAddedFiles->getFilePathList();

    QList<QFuture<bool>> resultList;

    auto localFSM = this->createFSM();

    for(const QString &currentFilePath : files)
    {
        QFuture<bool> result = QtConcurrent::run([&, this] {

            QFileInfo fileInfo(currentFilePath);
            QString userDirectory = QDir::toNativeSeparators(fileInfo.absolutePath()) + QDir::separator();

            bool requestResult = localFSM->addNewFile(currentFilePath,
                                                      this->targetSymbolFolder,
                                                      false,
                                                      true,
                                                      userDirectory
                                                      );
            return requestResult;
        });

        resultList.append(result);
    }

    for(QFuture<bool> current : resultList)
    {
        current.waitForFinished();
        qDebug() << "current = " << current.result();
    }

//    for(const QString &currentFilePath : files)
//    {
//        qDebug() << "current (in same thread) = " << this->postToFSM(currentFilePath);
//    }


    this->showStatusInfo("Everything complated");
}

