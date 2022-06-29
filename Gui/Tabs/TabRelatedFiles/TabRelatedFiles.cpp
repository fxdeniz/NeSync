#include "TabRelatedFiles.h"
#include "ui_TabRelatedFiles.h"

TabRelatedFiles::TabRelatedFiles(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabRelatedFiles)
{
    ui->setupUi(this);
}

TabRelatedFiles::~TabRelatedFiles()
{
    delete ui;
}
