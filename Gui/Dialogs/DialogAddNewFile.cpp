#include "DialogAddNewFile.h"
#include "ui_DialogAddNewFile.h"

#include <QStandardPaths>
#include <QStorageInfo>
#include <QFileDialog>

DialogAddNewFile::DialogAddNewFile(FileStorageManager *fsm, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAddNewFile)
{
    ui->setupUi(this);
    this->fileStorageManager = fsm;

    QList<TableModelNewAddedFiles::TableItem> sampleFileExplorerTableData;

    sampleFileExplorerTableData.insert(0, {"first_file", ".txt"});
    sampleFileExplorerTableData.insert(1, {"second_file", ".zip"});
    sampleFileExplorerTableData.insert(2, {"third_file", ".pdf"});
    sampleFileExplorerTableData.insert(3, {"fourth_file", ".mp4"});

    this->tableModelNewAddedFiles = new TableModelNewAddedFiles(sampleFileExplorerTableData, this);
    this->ui->tableView->setModel(this->tableModelNewAddedFiles);
}

DialogAddNewFile::~DialogAddNewFile()
{
    delete ui;
}

void DialogAddNewFile::on_buttonSelectNewFile_clicked()
{
    QFileDialog dialog(this);
    dialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::StandardLocation::DesktopLocation));
    dialog.setFileMode(QFileDialog::FileMode::ExistingFile);

    if(dialog.exec())
    {
        auto selectedFilePath = dialog.selectedFiles().at(0);

        QFile selectedFile(selectedFilePath);
        if(!selectedFile.exists())
        {
            this->showStatusWarning("File not found: " + selectedFilePath);
            return;
        }

        bool isFileOpened = selectedFile.open(QFile::OpenModeFlag::ReadOnly);

        if(!isFileOpened)
        {
            this->showStatusWarning("File couldn't opened: " + selectedFilePath);
            return;
        }

        QStorageInfo info(this->fileStorageManager->getBackupDirectory());
        auto fileSize = selectedFile.size();
        auto availableSize = info.bytesFree();

        if(fileSize > availableSize)
        {
            this->showStatusWarning("Not enough free space available for: " + selectedFilePath);
            return;
        }
    }
}

void DialogAddNewFile::showStatusWarning(const QString &message)
{
    auto labelStatus = this->ui->labelStatus;

    QPalette palette;
    palette.setColor(QPalette::ColorRole::Window, Qt::GlobalColor::yellow);
    palette.setColor(QPalette::ColorRole::WindowText, Qt::GlobalColor::black);

    labelStatus->setPalette(palette);
    labelStatus->setAutoFillBackground(true);
    labelStatus->setText(message);
}

void DialogAddNewFile::showStatusError(const QString &message)
{
    auto labelStatus = this->ui->labelStatus;

    QPalette palette;
    palette.setColor(QPalette::ColorRole::Window, Qt::GlobalColor::red);
    palette.setColor(QPalette::ColorRole::WindowText, Qt::GlobalColor::black);

    labelStatus->setPalette(palette);
    labelStatus->setAutoFillBackground(true);
    labelStatus->setText(message);
}

