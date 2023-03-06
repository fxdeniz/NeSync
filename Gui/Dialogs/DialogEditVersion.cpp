#include "DialogEditVersion.h"
#include "ui_DialogEditVersion.h"

#include "Backend/FileStorageSubSystem/FileStorageManager.h"
#include "Utility/JsonDtoFormat.h"

#include <QStringListModel>

DialogEditVersion::DialogEditVersion(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogEditVersion)
{
    ui->setupUi(this);
}

DialogEditVersion::~DialogEditVersion()
{
    delete ui;
}

void DialogEditVersion::show(const QString &fileSymbolPath, qlonglong versionNumber)
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

void DialogEditVersion::on_comboBox_currentTextChanged(const QString &arg1)
{
    showStatusNormal("", ui->labelStatus);

    auto fsm = FileStorageManager::instance();
    QJsonObject versionJson = fsm->getFileVersionJson(currentFileSymbolPath, arg1.toLongLong());
    auto description = versionJson[JsonKeys::FileVersion::Description].toString();
    ui->textEdit->setText(description);

    if(description.isEmpty())
        showStatusWarning("No description set for this version", ui->labelStatus);
}


void DialogEditVersion::on_buttonRestore_clicked()
{
    auto fsm = FileStorageManager::instance();
    QJsonObject versionJson = fsm->getFileVersionJson(currentFileSymbolPath, ui->comboBox->currentText().toLongLong());
    QString description = versionJson[JsonKeys::FileVersion::Description].toString();

    ui->textEdit->clear();
    ui->textEdit->setText(description);

    if(description.isEmpty())
        showStatusInfo("Description restored <b>(Description is empty because previous was one also empty)<b/>", ui->labelStatus);
    else
        showStatusInfo("Description restored", ui->labelStatus);
}


void DialogEditVersion::on_buttonSave_clicked()
{
    auto fsm = FileStorageManager::instance();
    QJsonObject versionJson = fsm->getFileVersionJson(currentFileSymbolPath, ui->comboBox->currentText().toLongLong());
    versionJson[JsonKeys::FileVersion::Description] = ui->textEdit->toPlainText();
    bool isUpdated = fsm->updateFileVersionEntity(versionJson);

    if(isUpdated)
        showStatusSuccess("Description updated for current version", ui->labelStatus);
    else
        showStatusError("Couldn't update the description, check the debug logs", ui->labelStatus);
}

