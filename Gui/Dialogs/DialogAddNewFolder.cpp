#include "DialogAddNewFolder.h"
#include "ui_DialogAddNewFolder.h"

DialogAddNewFolder::DialogAddNewFolder(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAddNewFolder)
{
    ui->setupUi(this);
}

DialogAddNewFolder::~DialogAddNewFolder()
{
    delete ui;
}
