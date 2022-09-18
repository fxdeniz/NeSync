#include "V2_DialogAddNewFolder.h"
#include "ui_V2_DialogAddNewFolder.h"

V2_DialogAddNewFolder::V2_DialogAddNewFolder(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::V2_DialogAddNewFolder)
{
    ui->setupUi(this);
}

V2_DialogAddNewFolder::~V2_DialogAddNewFolder()
{
    delete ui;
}
