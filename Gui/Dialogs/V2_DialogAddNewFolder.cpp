#include "V2_DialogAddNewFolder.h"
#include "ui_V2_DialogAddNewFolder.h"
#include "Backend/FileStorageSubSystem/FileStorageManager.h"

#include <QFileIconProvider>
#include <QStandardPaths>
#include <QHashIterator>
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
        ui->labelFolderName->setText("New Folder Name");

    ui->lineEditFolderPath->setFocus();
    ui->lineEditFolderPath->selectedText();

    QWidget::show();
}

void V2_DialogAddNewFolder::on_buttonSelectFolder_clicked()
{
    QObject::connect(model, &QFileSystemModel::rootPathChanged,
                     this, &V2_DialogAddNewFolder::slotEnableButtonAddFilesToDb);

    QFileDialog dialog(this);
    dialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::StandardLocation::DesktopLocation));
    dialog.setFileMode(QFileDialog::FileMode::Directory);

    if(dialog.exec())
    {
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

void V2_DialogAddNewFolder::createSymbolDirs()
{
    QString rootPath = QDir::toNativeSeparators(model->rootPath());
    QDir rootDir = model->rootDirectory();
    rootDir.setFilter(QDir::Filter::Dirs | QDir::Filter::NoDotAndDotDot);

    QDirIterator dirIterator(rootDir, QDirIterator::IteratorFlag::Subdirectories);
    QHash<QString, QString> userDirToSymbolDirMapping;
    QString parentSymbolDir =  ui->labelParentFolderPath->text() + ui->labelFolderName->text();
    userDirToSymbolDirMapping.insert(rootPath, parentSymbolDir);

    while(dirIterator.hasNext())
    {
        auto currentUserDir = dirIterator.next();
        currentUserDir = QDir::toNativeSeparators(currentUserDir);

        auto suffix = currentUserDir.split(rootPath).last();
        suffix = QDir::toNativeSeparators(suffix);
        suffix.prepend(parentSymbolDir);
        suffix.replace(QDir::separator(), FileStorageManager::CONST_SYMBOL_DIRECTORY_SEPARATOR);
        userDirToSymbolDirMapping.insert(currentUserDir, suffix);
    }

    QHashIterator<QString, QString> hashIterator(userDirToSymbolDirMapping);

    while(hashIterator.hasNext())
    {
        // QHashIterator starts from index -1
        //      see https://doc.qt.io/qt-6/qhashiterator.html#details
        hashIterator.next();
    }
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

QString V2_DialogAddNewFolder::statusTextExist(QString folderName)
{
    QString text = tr("Folder <b>%1</b> already exist");
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
    this->showStatusInfo(statusTextAdding(), ui->labelStatus);
    ui->progressBar->show();
    createSymbolDirs();
}

void V2_DialogAddNewFolder::slotEnableButtonAddFilesToDb(const QString &dummy)
{
    Q_UNUSED(dummy);
    ui->buttonAddFilesToDb->setEnabled(true);
    showStatusInfo(statusTextContentReadyToAdd(), ui->labelStatus);

    QObject::disconnect(model, &QFileSystemModel::rootPathChanged,
                        this, &V2_DialogAddNewFolder::slotEnableButtonAddFilesToDb);
}
