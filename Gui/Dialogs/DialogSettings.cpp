#include "DialogSettings.h"
#include "ui_DialogSettings.h"

#include "Utility/AppConfig.h"

DialogSettings::DialogSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSettings)
{
    ui->setupUi(this);

    AppConfig config;
    ui->lineEdit->setText(config.getStorageFolderPath());
}

DialogSettings::~DialogSettings()
{
    delete ui;
}
