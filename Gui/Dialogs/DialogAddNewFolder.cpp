#include "DialogAddNewFolder.h"
#include "ui_DialogAddNewFolder.h"

#include "Backend/FileStorageSubSystem/FileStorageManager.h"

#include <QFileIconProvider>

DialogAddNewFolder::DialogAddNewFolder(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAddNewFolder)
{
    ui->setupUi(this);

    QFileIconProvider iconProvider;
    auto pixmap = iconProvider.icon(QFileIconProvider::IconType::Folder).pixmap(24, 24);
    ui->labelFolderIcon->setPixmap(pixmap);
    ui->labelFolderIcon->setMask(pixmap.mask());
}

DialogAddNewFolder::~DialogAddNewFolder()
{
    delete ui;
}

void DialogAddNewFolder::show(const QString &_parentFolderPath)
{
    this->parentFolderPath = _parentFolderPath;
    ui->labelParentFolderPath->setText(_parentFolderPath);

    showStatusInfo(expectingStatusText(), ui->labelStatus);
    if(ui->lineEdit->text().isEmpty())
        ui->labelFolderName->setText("New Folder Name");

    ui->lineEdit->setFocus();
    ui->lineEdit->selectedText();

    QWidget::show();
}

void DialogAddNewFolder::on_pushButton_clicked()
{
    auto fsm = FileStorageManager::instance();
    QString lineEditText = ui->lineEdit->text();
    auto newFolderPath = parentFolderPath + lineEditText;
    bool isFolderExist = fsm->isFolderExist(newFolderPath);

    if(lineEditText.isEmpty() || lineEditText.isNull())
    {
        showStatusWarning(emptyFolderStatusText(), ui->labelStatus);
        return;
    }

    if(isFolderExist)
    {
        showStatusWarning(existStatusText(lineEditText), ui->labelStatus);
        return;
    }

    bool isFolderAdded = fsm->addNewFolder(newFolderPath);

    if(isFolderAdded)
    {
        showStatusSuccess(successStatusText(lineEditText), ui->labelStatus);
        ui->lineEdit->clear();
        ui->lineEdit->setFocus();

        if(ui->checkBox->isChecked())
            close();
    }
    else
        showStatusError(errorStatusText(lineEditText), ui->labelStatus);
}


void DialogAddNewFolder::on_lineEdit_textChanged(const QString &arg1)
{
    if(!arg1.isEmpty())
        ui->labelFolderName->setText(arg1);
    else
        ui->labelFolderName->setText("New Folder Name");
}

QString DialogAddNewFolder::expectingStatusText()
{
    return "Expecting a <b>new folder name</b>";
}

QString DialogAddNewFolder::emptyFolderStatusText()
{
    return "Folder name cannot be <b>empty</b>";
}

QString DialogAddNewFolder::existStatusText(QString folderName)
{
    QString text = "Folder <b>%1</b> already exist";
    text = text.arg(folderName);
    return text;
}

QString DialogAddNewFolder::successStatusText(QString folderName)
{
    QString text = "Folder <b>%1</b> created successfully";
    text = text.arg(folderName);
    return text;
}

QString DialogAddNewFolder::errorStatusText(QString folderName)
{
    QString text = "Error ocured while creating folder <b>%1</b>";
    text = text.arg(folderName);
    return text;
}

