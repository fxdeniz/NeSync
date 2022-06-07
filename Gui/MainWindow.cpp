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
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showContextMenuListView(const QPoint &pos)
{
    QModelIndex index=this->ui->listView->indexAt(pos);

    if(index.isValid()) // If user selected an item from list.
    {
        QMenu *ptrMenu = new QMenu(this);
        ptrMenu->addAction(new QAction("Action 1", ptrMenu));
        ptrMenu->addAction(new QAction("Action 2", ptrMenu));
        ptrMenu->addAction(new QAction("Action 3", ptrMenu));
        ptrMenu->popup(this->ui->listView->viewport()->mapToGlobal(pos));
    }
}
