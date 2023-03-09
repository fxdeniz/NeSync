#include "DialogExport.h"
#include "ui_DialogExport.h"

#include <QFileDialog>
#include <QStandardPaths>

DialogExport::DialogExport(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogExport)
{
    ui->setupUi(this);
}

DialogExport::~DialogExport()
{
    delete ui;
}

void DialogExport::show()
{
    ui->lineEdit->clear();
    ui->buttonExport->setDisabled(true);
    QWidget::show();
}

void DialogExport::on_buttonSelectLocation_clicked()
{
    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::StandardLocation::DesktopLocation);
    desktopPath = QDir::toNativeSeparators(desktopPath);
    desktopPath += QDir::separator();

    QString filePath = QFileDialog::getSaveFileName(this, tr("Save Zip File"),
                                                    desktopPath,
                                                    tr("Zip files (*.zip)"));

    if(filePath.isEmpty())
        return;

    ui->lineEdit->setText(filePath);
    ui->buttonExport->setEnabled(true);
}

