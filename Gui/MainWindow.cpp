#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->dialogTableItemEditor = new DialogTableItemEditor(this);

    QList<TableModelFileExplorer::TableItem> sampleFileExplorerTableData;

    sampleFileExplorerTableData.insert(0, {"first_file", ".txt"});
    sampleFileExplorerTableData.insert(1, {"second_file", ".zip"});
    sampleFileExplorerTableData.insert(2, {"third_file", ".pdf"});
    sampleFileExplorerTableData.insert(3, {"fourth_file", ".mp4"});

    this->tableModelFileExplorer = new TableModelFileExplorer(sampleFileExplorerTableData, this);
    this->ui->tableViewFileExplorer->setModel(this->tableModelFileExplorer);

    QStringList sampleListData;
    sampleListData << "item 1" << "item 2" << "item 3" << "item 4" << "item 5";

    this->listModelFileExplorer = new ListModelFileExplorer(sampleListData, this);
    this->ui->listView->setModel(this->listModelFileExplorer);

    QList<TableModelFileMonitor::MonitorTableItem> sampleFileMonitorTableData;

    sampleFileMonitorTableData.insert(0, {"first_file.txt", "C:/Users/<USER>/AppData/Roaming/<APPNAME>", "Deleted"});
    sampleFileMonitorTableData.insert(1, {"second_file.zip", 	"C:/Users/<USER>/Desktop", "Moved"});
    sampleFileMonitorTableData.insert(2, {"third_file.pdf", "C:/Users/<USER>/Desktop", "Updated"});
    sampleFileMonitorTableData.insert(3, {"fourth_file.mp4", "C:/Users/<USER>/Videos", "New Added"});

    this->tableModelFileMonitor = new TableModelFileMonitor(sampleFileMonitorTableData, this);
    this->ui->tableViewFileMonitor->setModel(this->tableModelFileMonitor);
    this->ui->tableViewFileMonitor->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
    this->ui->tableViewFileMonitor->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeMode::Interactive);
    this->ui->tableViewFileMonitor->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeMode::Interactive);

    this->comboBoxItemDelegateNote = new ComboBoxItemDelegateNote(this->ui->tableViewFileMonitor);
    this->comboBoxItemDelegateFileAction = new ComboBoxItemDelegateFileAction(this->ui->tableViewFileMonitor);

    for(int rowIndex = 0; rowIndex < sampleFileMonitorTableData.size(); rowIndex++)
    {
        int columnActionIndex = 3;
        int columnNoteIndex = 4;

        this->ui->tableViewFileMonitor->setItemDelegateForColumn(columnNoteIndex, this->comboBoxItemDelegateNote);
        this->ui->tableViewFileMonitor->openPersistentEditor(this->tableModelFileMonitor->index(rowIndex, columnNoteIndex));


        this->ui->tableViewFileMonitor->setItemDelegateForColumn(columnActionIndex, this->comboBoxItemDelegateFileAction);
        this->ui->tableViewFileMonitor->openPersistentEditor(this->tableModelFileMonitor->index(rowIndex, columnActionIndex));
    }


    this->ui->tableViewFileMonitor->resizeColumnsToContents();
    this->ui->tabWidget->setCurrentIndex(0);

    QObject::connect(this->ui->listView, &QListView::customContextMenuRequested, this, &MainWindow::showContextMenuListView);
    QObject::connect(this->ui->tableViewFileExplorer, &QTableView::customContextMenuRequested, this, &MainWindow::showContextMenuTableView);
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
        ptrMenu->addAction(new QAction("Preview", ptrMenu));
        ptrMenu->addAction(new QAction("Edit", ptrMenu));
        ptrMenu->addAction(new QAction("Schedule for independent copy", ptrMenu));
        ptrMenu->addAction(new QAction("Schedule and open indepented copy menu", ptrMenu));
        ptrMenu->addAction(new QAction("Delete", ptrMenu));

        ptrMenu->popup(subjectView->viewport()->mapToGlobal(argPos));
    }
}

void MainWindow::showContextMenuTableView(const QPoint &argPos)
{
    QAbstractItemView *subjectView = this->ui->tableViewFileExplorer;
    QModelIndex index = subjectView->indexAt(argPos);

    if(index.isValid()) // If user selected an item from table.
    {
        QMenu *ptrMenu = new QMenu(this->ui->tableViewFileExplorer);
        QAction *actionEdit = new QAction("Edit", ptrMenu);
        QAction *actionCut = new QAction("Cut", ptrMenu);
        QAction *actionFreeze = new QAction("Freeze", ptrMenu);
        QAction *actionDelete = new QAction("Delete", ptrMenu);

        ptrMenu->addAction(actionEdit);
        ptrMenu->addAction(actionCut);
        ptrMenu->addAction(actionFreeze);
        ptrMenu->addAction(actionDelete);

        QObject::connect(actionEdit, &QAction::triggered, this, &MainWindow::on_actionEditTableItem_clicked);

        ptrMenu->popup(subjectView->viewport()->mapToGlobal(argPos));
    }
}

void MainWindow::on_actionEditTableItem_clicked()
{
    this->dialogTableItemEditor->setModal(true);
    this->dialogTableItemEditor->show();
}

