#include "DialogSettings.h"
#include "ui_DialogSettings.h"

#include "Utility/AppConfig.h"

#include <QFileDialog>
#include <QStandardPaths>

DialogSettings::DialogSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSettings)
{
    ui->setupUi(this);
}

DialogSettings::~DialogSettings()
{
    delete ui;
}

void DialogSettings::show()
{
    AppConfig config;
    ui->lineEdit->setText(config.getStorageFolderPath());
    showStatusInfo(tr("No changes made made to settings yet."), ui->labelStatus);
    ui->buttonSave->setDisabled(true);

    QWidget::show();
}

void DialogSettings::on_buttonSelectStorageFolder_clicked()
{
    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::StandardLocation::DesktopLocation);
    desktopPath = QDir::toNativeSeparators(desktopPath);
    desktopPath += QDir::separator();

    QString folderPath = QFileDialog::getExistingDirectory(this, tr("Select a profile folder"), desktopPath);

    if(folderPath.isEmpty())
        return;

    folderPath = QDir::toNativeSeparators(folderPath) + QDir::separator();
    ui->lineEdit->setText(folderPath);

    bool isExist = QFile::exists(folderPath + "ns_database.db3");

    if(isExist)
    {
        QString message = tr("Existing profile found at selected location.<br>"
                             "After saving, selected profile will be loaded. <b>NOTE:</b> Saving requires restard.");

        showStatusSuccess(message, ui->labelStatus);
    }
    else
    {
        QString message = tr("No profile found at selected location. New profile will be created.<br>"
                             "After saving, new and empty profile will be loaded. <b>NOTE:</b> Saving requires restard.");

        showStatusWarning(message, ui->labelStatus);
    }

    ui->buttonSave->setEnabled(true);
}


void DialogSettings::on_buttonSave_clicked()
{
    AppConfig config;
    config.setStorageFolderPath(ui->lineEdit->text());
    qApp->exit(0);
}

