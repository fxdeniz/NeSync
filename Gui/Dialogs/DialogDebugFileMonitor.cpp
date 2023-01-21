#include "DialogDebugFileMonitor.h"
#include "ui_DialogDebugFileMonitor.h"

#include <QSqlQueryModel>

#include "Utility/DatabaseRegistry.h"

DialogDebugFileMonitor::DialogDebugFileMonitor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogDebugFileMonitor)
{
    ui->setupUi(this);

    database = DatabaseRegistry::inMemoryFileSystemEventDatabase();
    database.open();
}

DialogDebugFileMonitor::~DialogDebugFileMonitor()
{
    delete ui;
}

void DialogDebugFileMonitor::on_buttonRefresh_clicked()
{
    {
        bool isFolderModelNull = (ui->tableViewFolder->model() == nullptr);

        if(!isFolderModelNull)
            delete ui->tableViewFolder->model();

        QSqlQueryModel *folderModel = new QSqlQueryModel(this);
        folderModel->setQuery("SELECT * FROM " + DatabaseRegistry::fileSystemEventDbFolderTableName(),
                              database);

        ui->tableViewFolder->setModel(folderModel);
    }

    {
        bool isFileModelNull = (ui->tableViewFile->model() == nullptr);

        if(!isFileModelNull)
            delete ui->tableViewFile->model();

        QSqlQueryModel *fileModel = new QSqlQueryModel(this);
        fileModel->setQuery("SELECT * FROM " + DatabaseRegistry::fileSystemEventDbFileTableName(),
                              database);

        ui->tableViewFile->setModel(fileModel);
    }
}

