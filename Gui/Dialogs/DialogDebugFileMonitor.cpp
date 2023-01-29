#include "DialogDebugFileMonitor.h"
#include "ui_DialogDebugFileMonitor.h"

#include <QSqlQueryModel>

#include "Utility/DatabaseRegistry.h"

DialogDebugFileMonitor::DialogDebugFileMonitor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogDebugFileMonitor)
{
    ui->setupUi(this);

    database = DatabaseRegistry::fileSystemEventDatabase();
    database.open();
    ui->tabWidget->setCurrentIndex(0);
    ui->buttonExecute->setText("Refresh Folders");
}

DialogDebugFileMonitor::~DialogDebugFileMonitor()
{
    delete ui;
}

void DialogDebugFileMonitor::on_buttonExecute_clicked()
{
    int currentIndex = ui->tabWidget->currentIndex();
    if(currentIndex == 0)
    {
        bool isFolderModelNull = (ui->tableViewFolder->model() == nullptr);

        if(!isFolderModelNull)
            delete ui->tableViewFolder->model();

        QSqlQueryModel *folderModel = new QSqlQueryModel(this);
        folderModel->setQuery("SELECT * FROM Folder ORDER BY folder_path ASC", database);

        ui->tableViewFolder->setModel(folderModel);

        ui->tableViewFolder->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
        ui->tableViewFolder->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeMode::Interactive);
        ui->tableViewFolder->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeMode::Interactive);
        ui->tableViewFolder->horizontalHeader()->setMinimumSectionSize(80);
        ui->tableViewFolder->setColumnWidth(1, 250);
        ui->tableViewFolder->setColumnWidth(2, 250);
    }
    else if(currentIndex == 1)
    {
        bool isFileModelNull = (ui->tableViewFile->model() == nullptr);

        if(!isFileModelNull)
            delete ui->tableViewFile->model();

        QSqlQueryModel *fileModel = new QSqlQueryModel(this);
        fileModel->setQuery("SELECT * FROM File ORDER BY file_path ASC", database);

        ui->tableViewFile->setModel(fileModel);

        ui->tableViewFile->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
        ui->tableViewFile->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeMode::Interactive);
        ui->tableViewFile->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeMode::Interactive);
        ui->tableViewFile->horizontalHeader()->setMinimumSectionSize(80);
        ui->tableViewFile->setColumnWidth(0, 300);
        ui->tableViewFile->setColumnWidth(1, 250);
    }
    else if(currentIndex == 2)
    {
        bool isMonitroingErrorModelNull = (ui->tableViewMonitoringError->model() == nullptr);

        if(!isMonitroingErrorModelNull)
            delete ui->tableViewMonitoringError->model();

        QSqlQueryModel *errorModel = new QSqlQueryModel(this);
        errorModel->setQuery("SELECT * FROM MonitoringError ORDER BY location ASC, event_timestamp ASC", database);

        ui->tableViewMonitoringError->setModel(errorModel);

        ui->tableViewMonitoringError->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
        ui->tableViewMonitoringError->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeMode::Interactive);
        ui->tableViewMonitoringError->horizontalHeader()->setMinimumSectionSize(80);
        ui->tableViewMonitoringError->setColumnWidth(0, 300);
    }
    else if(currentIndex == 3)
    {
        bool isQueryModelNull = (ui->tableViewQuery->model() == nullptr);

        if(!isQueryModelNull)
            delete ui->tableViewQuery->model();

        QSqlQueryModel *queryModel = new QSqlQueryModel(this);
        queryModel->setQuery(ui->textEditQuery->toPlainText(), database);

        ui->tableViewQuery->setModel(queryModel);
    }
}


void DialogDebugFileMonitor::on_tabWidget_currentChanged(int index)
{
    if(index == 0)
        ui->buttonExecute->setText("Refresh Folders");
    else if(index == 1)
        ui->buttonExecute->setText("Refresh Files");
    else if(index == 2)
        ui->buttonExecute->setText("Refresh Monitoring Errors");
    else if(index == 3)
        ui->buttonExecute->setText("Execute Query");
}

