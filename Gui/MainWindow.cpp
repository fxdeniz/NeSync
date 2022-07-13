#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "Tabs/TabRelatedFiles.h"

#include <QStandardPaths>
#include <QtConcurrent>
#include <QDir>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    auto appDataDir = QStandardPaths::writableLocation(QStandardPaths::StandardLocation::TempLocation);
    appDataDir = QDir::toNativeSeparators(appDataDir);
    appDataDir += QDir::separator();

    auto backupDir = appDataDir + "backup" + QDir::separator();
    auto symbolDir = appDataDir + "symbols" + QDir::separator();
    QDir dir;
    dir.mkdir(backupDir);
    dir.mkdir(symbolDir);

    dialogAddNewFolder = new DialogAddNewFolder(this);
    dialogTableItemEditor = new DialogFileOrDirEditor(this);
    dialogAddNewFile = new DialogAddNewFile(this);

    allocateSeparators();
    buildTabWidget();
    disableCloseButtonOfPredefinedTabs();
    on_tabWidget_currentChanged(ui->tabWidget->currentIndex());

    ui->tabWidget->setCurrentIndex(0);

    QObject::connect(dialogAddNewFolder, &QDialog::accepted,
                     tabFileExplorer, &TabFileExplorer::slotRefreshFileExplorer);

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
    QToolBar *toolBar = ui->toolBar;
    toolBar->clear();

    if(index == 0)
    {
        toolBar->addAction(ui->tab1Action_NewFolder);
        toolBar->addAction(ui->tab1Action_AddFile);
        toolBar->addAction(separator1);

        toolBar->addAction(ui->tab1Action_SelectAll);
        toolBar->addAction(ui->tab1Action_UnSelectAll);
        toolBar->addAction(separator2);

        toolBar->addAction(ui->tab1Action_PasteHere);
        toolBar->addAction(ui->tab1Action_ViewClipboard);
        toolBar->addAction(separator3);

        toolBar->addAction(ui->tab1Action_Import);
        toolBar->addAction(ui->tab1Action_Export);
    }
    else if(index == 1)
    {
        toolBar->addAction(ui->tab2Action_SaveAll);
        toolBar->addAction(ui->tab2Action_SaveSelected);
    }
    else // If TabRelatedFiles accessed at runtime.
    {
        toolBar->addAction(ui->tabRelatedFilesAction_Refresh);
    }
}

void MainWindow::allocateSeparators()
{
    separator1 = new QAction(this);
    separator1->setSeparator(true);

    separator2 = new QAction(this);
    separator2->setSeparator(true);

    separator3 = new QAction(this);
    separator3->setSeparator(true);
}

void MainWindow::buildTabWidget()
{
    tabFileExplorer = new TabFileExplorer(ui->tabWidget);
    tabFileMonitor = new TabFileMonitor(ui->tabWidget);

    ui->tabWidget->addTab(tabFileExplorer, "File Explorer");
    ui->tabWidget->addTab(tabFileMonitor, "File Monitor");
}

void MainWindow::disableCloseButtonOfPredefinedTabs()
{
    QTabBar *tabBar = ui->tabWidget->tabBar();
    tabBar->tabButton(0, QTabBar::ButtonPosition::RightSide)->deleteLater();
    tabBar->setTabButton(0, QTabBar::ButtonPosition::RightSide, nullptr);

    tabBar->tabButton(1, QTabBar::ButtonPosition::RightSide)->deleteLater();
    tabBar->setTabButton(1, QTabBar::ButtonPosition::RightSide, nullptr);
}


void MainWindow::on_router_ShowRelatedFiles()
{
    TabRelatedFiles *tab = new TabRelatedFiles(ui->tabWidget);
    QTabWidget *tabWidget = ui->tabWidget;
    tabWidget->addTab(tab, "Related Files");

    QObject::connect(tabWidget->tabBar(), &QTabBar::tabCloseRequested, tabWidget->tabBar(), &QTabBar::removeTab);
}

void MainWindow::on_router_ShowDialogTableItemEditor()
{
    dialogTableItemEditor->setModal(true);
    dialogTableItemEditor->show();
}


void MainWindow::on_tab1Action_AddFile_triggered()
{
    Qt::WindowFlags flags = dialogAddNewFile->windowFlags();
    flags |= Qt::WindowMaximizeButtonHint;
    dialogAddNewFile->setWindowFlags(flags);
    dialogAddNewFile->setModal(true);
    dialogAddNewFile->show(tabFileExplorer->currentDir());
}


void MainWindow::on_tab1Action_NewFolder_triggered()
{
    dialogAddNewFolder->setModal(true);
    dialogAddNewFolder->show(tabFileExplorer->currentDir());
}
