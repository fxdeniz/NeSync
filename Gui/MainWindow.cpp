#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QList<UserContentTableModel::TableItem> sampleTableData;

    sampleTableData.insert(0, {"first_file", ".txt"});
    sampleTableData.insert(1, {"second_file", ".zip"});
    sampleTableData.insert(2, {"third_file", ".pdf"});
    sampleTableData.insert(3, {"fourth_file", ".mp4"});

    this->userContentTableModel = new UserContentTableModel(sampleTableData, this);
    this->ui->tableView->setModel(this->userContentTableModel);


    QStringList sampleListData;
    sampleListData << "item 1" << "item 2" << "item 3" << "item 4" << "item 5";

    this->userContentListModel = new UserContentListModel(sampleListData, this);
    this->ui->listView->setModel(this->userContentListModel);

    QObject::connect(this->ui->listView, &QListView::customContextMenuRequested, this, &MainWindow::showContextMenuListView);
    QObject::connect(this->ui->tableView, &QTableView::customContextMenuRequested, this, &MainWindow::showContextMenuTableView);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showContextMenuListView(const QPoint &argPos)
{
    QAbstractItemView *subjectView = this->ui->listView;
    QModelIndex index = subjectView->indexAt(argPos);

    if(index.isValid()) // If user selected an item from list.
    {
        QMenu *ptrMenu = new QMenu(this->ui->listView);
        ptrMenu->addAction(new QAction("Edit", ptrMenu));
        ptrMenu->addAction(new QAction("Delete", ptrMenu));

        ptrMenu->popup(subjectView->viewport()->mapToGlobal(argPos));
    }
}

void MainWindow::showContextMenuTableView(const QPoint &argPos)
{
    QAbstractItemView *subjectView = this->ui->tableView;
    QModelIndex index = subjectView->indexAt(argPos);

    if(index.isValid()) // If user selected an item from table.
    {
        QMenu *ptrMenu = new QMenu(this->ui->tableView);
        ptrMenu->addAction(new QAction("Rename", ptrMenu));
        ptrMenu->addAction(new QAction("Cut", ptrMenu));
        ptrMenu->addAction(new QAction("Delete", ptrMenu));

        ptrMenu->popup(subjectView->viewport()->mapToGlobal(argPos));
    }
}

void MainWindow::on_buttonSelectControl_clicked()
{
    this->ui->tableView->selectAll();
}

