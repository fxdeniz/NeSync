#include "V2_DialogAddNewFolder.h"
#include "ui_V2_DialogAddNewFolder.h"

#include <QStandardPaths>
#include <QFileDialog>

V2_DialogAddNewFolder::V2_DialogAddNewFolder(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::V2_DialogAddNewFolder)
{
    ui->setupUi(this);
    model = new CustomFileSystemModel(this);
    //model->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
}

V2_DialogAddNewFolder::~V2_DialogAddNewFolder()
{
    delete ui;
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
    }
}


void V2_DialogAddNewFolder::on_treeView_doubleClicked(const QModelIndex &index)
{
    model->updateAutoSyncStatusOfItem(index);
}

