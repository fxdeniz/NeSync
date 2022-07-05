#include "DialogAddNewFile.h"
#include "ui_DialogAddNewFile.h"

#include <QStandardPaths>
#include <QFileDialog>

DialogAddNewFile::DialogAddNewFile(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAddNewFile)
{
    ui->setupUi(this);

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
    dialog.setFileMode(QFileDialog::FileMode::ExistingFiles);

    if(dialog.exec())
    {
        auto selectedFiles = dialog.selectedFiles();

        for(const QString &currentFilePath : selectedFiles)
        {
            QFile currentFile(currentFilePath);
            if(!currentFile.exists())
            {
                this->showStatusWarning("File not found: " + currentFilePath);
                return;
            }

            bool isFileOpened = currentFile.open(QFile::OpenModeFlag::ReadOnly);

            if(!isFileOpened)
            {
                this->showStatusWarning("File couldn't opened: " + currentFilePath);
                return;
            }
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

