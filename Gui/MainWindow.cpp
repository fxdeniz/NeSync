#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->dialogTableItemEditor = new DialogTableItemEditor(this);

    this->allocateSeparators();
    this->buildContextMenuTableFileExplorer();
    this->buildContextMenuListFileExplorer();
    this->on_tabWidget_currentChanged(this->ui->tabWidget->currentIndex());

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
        QMenu *ptrMenu = this->contextMenuListFileExplorer;
        ptrMenu->popup(subjectView->viewport()->mapToGlobal(argPos));
    }
}

void MainWindow::showContextMenuTableView(const QPoint &argPos)
{
    QAbstractItemView *subjectView = this->ui->tableViewFileExplorer;
    QModelIndex index = subjectView->indexAt(argPos);

    if(index.isValid()) // If user selected an item from table.
    {
        QMenu *ptrMenu = this->contextMenuTableFileExplorer;
        ptrMenu->popup(subjectView->viewport()->mapToGlobal(argPos));
    }
}

void MainWindow::on_actionEditTableItem_clicked()
{
    this->dialogTableItemEditor->setModal(true);
    this->dialogTableItemEditor->show();
}


void MainWindow::on_tabWidget_currentChanged(int index)
{
    QToolBar *toolBar = this->ui->toolBar;
    toolBar->clear();

    if(index == 0)
    {
        toolBar->addAction(this->ui->tab1Action_NewFolder);
        toolBar->addAction(this->ui->tab1Action_AddFile);
        toolBar->addAction(this->separator1);

        toolBar->addAction(this->ui->tab1Action_SelectAll);
        toolBar->addAction(this->ui->tab1Action_UnSelectAll);
        toolBar->addAction(this->separator2);

        toolBar->addAction(this->ui->tab1Action_PasteHere);
        toolBar->addAction(this->ui->tab1Action_ViewClipboard);
        toolBar->addAction(this->separator3);

        toolBar->addAction(this->ui->tab1Action_Import);
        toolBar->addAction(this->ui->tab1Action_Export);
    }
    else if(index == 1)
    {
        toolBar->addAction(this->ui->tab2Action_SaveAll);
        toolBar->addAction(this->ui->tab2Action_SaveSelected);
    }
}

void MainWindow::allocateSeparators()
{
    this->separator1 = new QAction(this);
    this->separator1->setSeparator(true);

    this->separator2 = new QAction(this);
    this->separator2->setSeparator(true);

    this->separator3 = new QAction(this);
    this->separator3->setSeparator(true);
}

void MainWindow::buildContextMenuTableFileExplorer()
{
    this->contextMenuTableFileExplorer = new QMenu(this->ui->tableViewFileExplorer);
    QMenu *ptrMenu = this->contextMenuTableFileExplorer;

    QAction *actionEdit = this->ui->contextActionTableFileExplorer_Edit;
    QAction *actionCut = this->ui->contextActionTableFileExplorer_Cut;
    QAction *actionFreeze = this->ui->contextActionTableFileExplorer_Freeze;
    QAction *actionDelete = this->ui->contextActionTableFileExplorer_Delete;

    ptrMenu->addAction(actionEdit);
    ptrMenu->addAction(actionCut);
    ptrMenu->addAction(actionFreeze);
    ptrMenu->addAction(actionDelete);

    QObject::connect(actionEdit, &QAction::triggered, this, &MainWindow::on_actionEditTableItem_clicked);
}

void MainWindow::buildContextMenuListFileExplorer()
{
    this->contextMenuListFileExplorer = new QMenu(this->ui->listView);
    QMenu *ptrMenu = this->contextMenuListFileExplorer;

    QAction *actionPreview = this->ui->contextActionListFileExplorer_Preview;
    QAction *actionEditVersion = this->ui->contextActionListFileExplorer_EditVersion;
    QAction *actionSchedule = this->ui->contextActionListFileExplorer_Schedule;
    QAction *actionScheduleAndOpenClipboard = this->ui->contextActionListFileExplorer_ScheduleAndOpenClipboard;
    QAction *actionDelete = this->ui->contextActionListFileExplorer_Delete;

    ptrMenu->addAction(actionPreview);
    ptrMenu->addAction(actionEditVersion);
    ptrMenu->addAction(actionSchedule);
    ptrMenu->addAction(actionScheduleAndOpenClipboard);
    ptrMenu->addAction(actionDelete);
}

