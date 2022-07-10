#include "DialogAddNewFolder.h"
#include "ui_DialogAddNewFolder.h"

#include "Backend/FileStorageSubSystem/FileStorageManager.h"

#include <QFileIconProvider>

DialogAddNewFolder::DialogAddNewFolder(const QString &parentFolderPath, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAddNewFolder)
{
    ui->setupUi(this);
    this->parentFolderPath = parentFolderPath;

    this->ui->labelParentFolderPath->setText(parentFolderPath);

    QFileIconProvider iconProvider;
    auto pixmap = iconProvider.icon(QFileIconProvider::IconType::Folder).pixmap(24, 24);
    this->ui->labelFolderIcon->setPixmap(pixmap);
    this->ui->labelFolderIcon->setMask(pixmap.mask());
}

DialogAddNewFolder::~DialogAddNewFolder()
{
    delete ui;
}

void DialogAddNewFolder::on_pushButton_clicked()
{
    auto fsm = FileStorageManager::instance();
    auto newFolderPath = this->parentFolderPath + this->ui->lineEdit->text();
    bool isFolderExist = fsm->isFolderExist(newFolderPath);

    if(isFolderExist)
    {
        this->ui->labelStatus->setText("Folder already exist");
        return;
    }

    bool isFolderAdded = fsm->addNewFolder(newFolderPath);

    if(isFolderAdded)
        this->ui->labelStatus->setText("Folder created successfully");
    else
        this->ui->labelStatus->setText("Error ocured while creating folder");
}


void DialogAddNewFolder::on_lineEdit_textChanged(const QString &arg1)
{
    this->ui->labelFolderName->setText(arg1);
}

