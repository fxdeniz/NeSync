#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "TabRelatedFiles.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->dialogTableItemEditor = new DialogTableItemEditor(this);

    this->allocateSeparators();
    this->buildTabWidget();
    this->disableCloseButtonOfPredefinedTabs();
    this->on_tabWidget_currentChanged(this->ui->tabWidget->currentIndex());

    this->ui->tabWidget->setCurrentIndex(0);

    TabFileExplorer *tabFileExplorer = this->tabFileExplorer;

    QObject::connect(tabFileExplorer, &TabFileExplorer::signalToRouter_ShowRelatedFiles,
                     this, &MainWindow::on_router_ShowRelatedFiles);

    QObject::connect(tabFileExplorer, &TabFileExplorer::signalToRouter_ShowDialogTableItemEditor,
                     this, &MainWindow::on_router_ShowDialogTableItemEditor);
}

MainWindow::~MainWindow()
{
    delete ui;
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
    else // If TabRelatedFiles accessed at runtime.
    {
        toolBar->addAction(this->ui->tabRelatedFilesAction_Refresh);
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

void MainWindow::buildTabWidget()
{
    this->tabFileExplorer = new TabFileExplorer(this->ui->tabWidget);
    this->tabFileMonitor = new TabFileMonitor(this->ui->tabWidget);

    this->ui->tabWidget->addTab(tabFileExplorer, "File Explorer");
    this->ui->tabWidget->addTab(tabFileMonitor, "File Monitor");
}

void MainWindow::disableCloseButtonOfPredefinedTabs()
{
    QTabBar *tabBar = this->ui->tabWidget->tabBar();
    tabBar->tabButton(0, QTabBar::ButtonPosition::RightSide)->deleteLater();
    tabBar->setTabButton(0, QTabBar::ButtonPosition::RightSide, nullptr);

    tabBar->tabButton(1, QTabBar::ButtonPosition::RightSide)->deleteLater();
    tabBar->setTabButton(1, QTabBar::ButtonPosition::RightSide, nullptr);
}


void MainWindow::on_router_ShowRelatedFiles()
{
    TabRelatedFiles *tab = new TabRelatedFiles(this->ui->tabWidget);
    QTabWidget *tabWidget = this->ui->tabWidget;
    tabWidget->addTab(tab, "Related Files");

    QObject::connect(tabWidget->tabBar(), &QTabBar::tabCloseRequested, tabWidget->tabBar(), &QTabBar::removeTab);
}

void MainWindow::on_router_ShowDialogTableItemEditor()
{
    this->dialogTableItemEditor->setModal(true);
    this->dialogTableItemEditor->show();
}

