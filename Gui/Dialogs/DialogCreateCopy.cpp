#include "DialogCreateCopy.h"
#include "ui_DialogCreateCopy.h"

#include "Utility/JsonDtoFormat.h"
#include "Backend/FileStorageSubSystem/FileStorageManager.h"

#include <QFileDialog>
#include <QStandardPaths>
#include <QStringListModel>

DialogCreateCopy::DialogCreateCopy(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogCreateCopy)
{
    ui->setupUi(this);
}

DialogCreateCopy::~DialogCreateCopy()
{
    delete ui;
}

void DialogCreateCopy::show(const QString &fileSymbolPath, qlonglong versionNumber)
{
    currentFileSymbolPath = fileSymbolPath;

    auto fsm = FileStorageManager::instance();
    QJsonObject fileJson = fsm->getFileJsonBySymbolPath(fileSymbolPath);
    qlonglong maxVersionNumber = fileJson[JsonKeys::File::MaxVersionNumber].toInteger();
    QStringList versionNumberList;

    for(qlonglong number = 1; number <= maxVersionNumber; number++)
        versionNumberList.append(QString::number(number));

    if(ui->comboBox->model() != nullptr)
        delete ui->comboBox->model();

    auto model = new QStringListModel(ui->comboBox);
    model->setStringList(versionNumberList);
    ui->comboBox->setModel(model);
    ui->comboBox->setCurrentIndex(versionNumber - 1);
    ui->labelFileName->setText(fileJson[JsonKeys::File::FileName].toString());

    QWidget::show();
}

void DialogCreateCopy::on_buttonSelectLocation_clicked()
{
    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::StandardLocation::DesktopLocation);
    QString selection = QFileDialog::getExistingDirectory(this,
                                                          tr("Select location for independent copy"),
                                                          desktopPath,
                                                          QFileDialog::Option::ShowDirsOnly | QFileDialog::Option::DontResolveSymlinks);
}

