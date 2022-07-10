#include "DialogAddNewFolder.h"
#include "ui_DialogAddNewFolder.h"

#include "Backend/FileStorageSubSystem/FileStorageManager.h"

#include <QFileIconProvider>

DialogAddNewFolder::DialogAddNewFolder(const QString &parentFolderPath, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAddNewFolder)
{
    ui->setupUi(this);
    this->labelStatus = this->ui->labelStatus;
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
    QString lineEditText = this->ui->lineEdit->text();
    auto newFolderPath = this->parentFolderPath + lineEditText;
    bool isFolderExist = fsm->isFolderExist(newFolderPath);

    if(lineEditText.isEmpty() || lineEditText.isNull())
    {
        this->showStatusWarning(this->emptyFolderStatusText(), this->labelStatus);
        return;
    }

    if(isFolderExist)
    {
        this->showStatusWarning(this->existStatusText(lineEditText), this->labelStatus);
        return;
    }

    bool isFolderAdded = fsm->addNewFolder(newFolderPath);

    if(isFolderAdded)
    {
        this->showStatusSuccess(this->successStatusText(lineEditText), this->labelStatus);
        this->ui->lineEdit->clear();
        this->ui->lineEdit->setFocus();

        if(this->ui->checkBox->isChecked())
            this->close();
    }
    else
        this->showStatusError(this->errorStatusText(lineEditText), this->labelStatus);
}


void DialogAddNewFolder::on_lineEdit_textChanged(const QString &arg1)
{
    if(!arg1.isEmpty())
        this->ui->labelFolderName->setText(arg1);
    else
        this->ui->labelFolderName->setText("New Folder Name");
}

void DialogAddNewFolder::show()
{
    this->showStatusInfo(this->expectingStatusText(), this->labelStatus);
    if(this->ui->lineEdit->text().isEmpty())
        this->ui->labelFolderName->setText("New Folder Name");

    this->ui->lineEdit->setFocus();
    this->ui->lineEdit->selectedText();

    QWidget::show();
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

