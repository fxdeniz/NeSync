#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QList<UserContentTableModel::TableItem> sampleData;

    sampleData.insert(0, {"first_file", ".txt"});
    sampleData.insert(1, {"second_file", ".zip"});
    sampleData.insert(2, {"third_file", ".pdf"});
    sampleData.insert(3, {"fourth_file", ".mp4"});

    this->userContentTableModel = new UserContentTableModel(sampleData, this);
    this->ui->tableView->setModel(this->userContentTableModel);
}

MainWindow::~MainWindow()
{
    delete ui;
}
