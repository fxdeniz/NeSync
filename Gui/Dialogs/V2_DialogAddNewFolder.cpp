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
    auto pixmap = iconProvider.icon(QFileIconProvider::IconType::Folder).pixmap(24, 24);
    ui->labelFolderIcon->setPixmap(pixmap);
    ui->labelFolderIcon->setMask(pixmap.mask());

    model = new CustomFileSystemModel(this);
    //model->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
}

V2_DialogAddNewFolder::~V2_DialogAddNewFolder()
{
    delete ui;
}

void V2_DialogAddNewFolder::show(const QString &_parentFolderPath)
{
    this->parentFolderPath = _parentFolderPath;
    ui->labelParentFolderPath->setText(_parentFolderPath);

    showStatusInfo(expectingStatusText(), ui->labelStatus);
    if(ui->lineEditFolderPath->text().isEmpty())
        ui->labelFolderName->setText("New Folder Name");

    ui->lineEditFolderPath->setFocus();
    ui->lineEditFolderPath->selectedText();

    QWidget::show();
}

void V2_DialogAddNewFolder::on_buttonSelectFolder_clicked()
{
    QFileDialog dialog(this);
    dialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::StandardLocation::DesktopLocation));
    dialog.setFileMode(QFileDialog::FileMode::Directory);

    if(dialog.exec())
    {
        ui->lineEditFolderPath->setText(dialog.selectedFiles().at(0));
        QModelIndex rootIndex = model->setRootPath(ui->lineEditFolderPath->text());        
        ui->treeView->setModel(model);
        ui->treeView->setRootIndex(rootIndex);

        ui->treeView->hideColumn(CustomFileSystemModel::ColumnIndex::Size);
        ui->treeView->hideColumn(CustomFileSystemModel::ColumnIndex::Type);
        ui->treeView->hideColumn(CustomFileSystemModel::ColumnIndex::DateModified);

        QDir selectedUserDir(ui->lineEditFolderPath->text());
        ui->labelFolderName->setText(selectedUserDir.dirName());
    }
}


void V2_DialogAddNewFolder::on_treeView_doubleClicked(const QModelIndex &index)
{
    model->updateAutoSyncStatusOfItem(index);
}

QString V2_DialogAddNewFolder::expectingStatusText()
{
    return "Expecting a <b>new folder name</b>";
}

QString V2_DialogAddNewFolder::emptyFolderStatusText()
{
    return "Folder name cannot be <b>empty</b>";
}

QString V2_DialogAddNewFolder::existStatusText(QString folderName)
{
    QString text = "Folder <b>%1</b> already exist";
    text = text.arg(folderName);
    return text;
}

QString V2_DialogAddNewFolder::successStatusText(QString folderName)
{
    QString text = "Folder <b>%1</b> created successfully";
    text = text.arg(folderName);
    return text;
}

QString V2_DialogAddNewFolder::errorStatusText(QString folderName)
{
    QString text = "Error ocured while creating folder <b>%1</b>";
    text = text.arg(folderName);
    return text;
}



void V2_DialogAddNewFolder::on_buttonTest_V2_RowFolderRecord_clicked()
{

    QString rootPath = QDir::toNativeSeparators(model->rootPath());
    QDir rootDir = model->rootDirectory();
    rootDir.setFilter(QDir::Filter::Dirs | QDir::Filter::NoDotAndDotDot);

    QDirIterator dirIterator(rootDir, QDirIterator::IteratorFlag::Subdirectories);
    QHash<QString, QString> userDirSuffixes;

    while(dirIterator.hasNext())
    {
        auto current = dirIterator.next();
        current = QDir::toNativeSeparators(current);

        auto suffix = current.split(rootPath).last();
        suffix = QDir::toNativeSeparators(suffix);
        userDirSuffixes.insert(current, suffix);
    }

    auto fsm = FileStorageManager::instance();
    QString parentSymbolDir =  ui->labelParentFolderPath->text() + ui->labelFolderName->text();
    fsm->addNewFolder(parentSymbolDir, rootPath);

    QHashIterator<QString, QString> hashIterator(userDirSuffixes);

    while(hashIterator.hasNext())
    {
        // QHashIterator starts from index -1
        //      see https://doc.qt.io/qt-6/qhashiterator.html#details
        hashIterator.next();

        auto currentSymbolDir = parentSymbolDir + hashIterator.value();
        currentSymbolDir.replace(QDir::separator(), FileStorageManager::CONST_SYMBOL_DIRECTORY_SEPARATOR);
        fsm->addNewFolder(currentSymbolDir, hashIterator.key());
    }
}

