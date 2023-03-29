#include "DialogCreateCopy.h"
#include "ui_DialogCreateCopy.h"

#include "Utility/JsonDtoFormat.h"
#include "Backend/FileStorageSubSystem/FileStorageManager.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QtConcurrent>
#include <QFutureWatcher>
#include <QStandardPaths>
#include <QProgressDialog>
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

    QString message = tr("Please select a folder");
    showStatusInfo(message, ui->labelStatus);
    ui->buttonCreateCopy->setDisabled(true);

    QWidget::show();
}

void DialogCreateCopy::on_buttonSelectLocation_clicked()
{
    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::StandardLocation::DesktopLocation);
    QString selection = QFileDialog::getExistingDirectory(this,
                                                          tr("Select folder for copy"),
                                                          desktopPath,
                                                          QFileDialog::Option::ShowDirsOnly | QFileDialog::Option::DontResolveSymlinks);

    if(!selection.isEmpty()) // If user selected folder
    {
        selection = QDir::toNativeSeparators(selection);
        if(!selection.endsWith(QDir::separator()))
            selection.append(QDir::separator());

        QString message;
        auto fsm = FileStorageManager::instance();
        QJsonObject folderJson = fsm->getFolderJsonByUserPath(selection);
        QString userFilePath = selection + ui->labelFileName->text();

        // If folder is not exist or folder is frozen
        if(!folderJson[JsonKeys::IsExist].toBool() || folderJson[JsonKeys::Folder::IsFrozen].toBool())
        {
            bool isFileExist = QFile::exists(userFilePath);

            if(isFileExist)
            {
                QString title = tr("File exist at selected folder !");
                QString message = tr("Can't overwrite file <b>%1</b> in folder you selected.");
                message = message.arg(ui->labelFileName->text());
                QMessageBox::critical(this, title, message);
                return;
            }

            message = tr("You've selected <b> not monitored folder</b>,"
                         " this means, copy you will create will not be monitored.");
            showStatusInfo(message, ui->labelStatus);
        }
        else
        {
            message = tr("You've selected <b>monitored folder</b>, when you create copy of file in this folder,"
                         " you will be asked for action in file monitor.");
            showStatusWarning(message, ui->labelStatus);
        }

        ui->lineEdit->setText(selection);
        ui->buttonCreateCopy->setEnabled(true);
    }
}


void DialogCreateCopy::on_buttonCreateCopy_clicked()
{
    QString userFilePath = ui->lineEdit->text() + ui->labelFileName->text();

    QFutureWatcher<void> futureWatcher;
    QProgressDialog dialog(this);
    dialog.setLabelText(tr("Creating copy from version <b>%1</b> of file <b>%2</b>...")
                        .arg(ui->comboBox->currentText().toInt())
                        .arg(ui->labelFileName->text())
                       );

    QObject::connect(&futureWatcher, &QFutureWatcher<void>::finished, &dialog, &QProgressDialog::reset);
    QObject::connect(&dialog, &QProgressDialog::canceled, &futureWatcher, &QFutureWatcher<void>::cancel);
    QObject::connect(&futureWatcher,  &QFutureWatcher<void>::progressRangeChanged, &dialog, &QProgressDialog::setRange);
    QObject::connect(&futureWatcher, &QFutureWatcher<void>::progressValueChanged,  &dialog, &QProgressDialog::setValue);

    bool isCopied = false;

    QFuture<void> future = QtConcurrent::run([=, &isCopied] {

        auto fsm = FileStorageManager::instance();
        QJsonObject fileJson = fsm->getFileVersionJson(currentFileSymbolPath, ui->comboBox->currentText().toInt());

        QString internalFilePath = fsm->getStorageFolderPath();
        internalFilePath += fileJson[JsonKeys::FileVersion::InternalFileName].toString();

        QFile::remove(userFilePath);
        isCopied = QFile::copy(internalFilePath, userFilePath);
    });

    futureWatcher.setFuture(future);
    dialog.exec();
    futureWatcher.waitForFinished();

    if(isCopied)
        showStatusSuccess(tr("Copy generated successfully"), ui->labelStatus);
    else
        showStatusError(tr("Couldn't generate copy, see error log"), ui->labelStatus);
}

