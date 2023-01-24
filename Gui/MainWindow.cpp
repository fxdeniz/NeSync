#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "Backend/FileStorageSubSystem/FileStorageManager.h"
#include "Tabs/TabRelatedFiles.h"

#include <QStandardPaths>
#include <QtConcurrent>
#include <QTabBar>
#include <QDir>

#include "Backend/FileMonitorSubSystem/V2_FileMonitoringManager.h"
#include "Utility/DatabaseRegistry.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QThread::currentThread()->setObjectName(guiThreadName());

    auto appDataDir = QStandardPaths::writableLocation(QStandardPaths::StandardLocation::TempLocation);
    appDataDir = QDir::toNativeSeparators(appDataDir);
    appDataDir += QDir::separator();

    auto backupDir = appDataDir + "backup" + QDir::separator();
    auto symbolDir = appDataDir + "symbols" + QDir::separator();
    QDir dir;
    dir.mkdir(backupDir);
    dir.mkdir(symbolDir);

    dialogTableItemEditor = new DialogFileOrDirEditor(this);
    dialogAddNewFolder = new DialogAddNewFolder(this);
    dialogDebugFileMonitor = new DialogDebugFileMonitor(this);

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

    createV2_FileMonitorThread();
}

MainWindow::~MainWindow()
{
    V2_fileMonitorThread->quit();
    V2_fileMonitorThread->wait();

    delete ui;
}

QString MainWindow::guiThreadName() const
{
    return "GUI Thread";
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    QToolBar *toolBar = ui->toolBar;
    toolBar->clear();

    if(index == 0)
    {
        toolBar->addAction(ui->tab1Action_AddNewFolder);
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
    V2_tabFileMonitor = new V2_TabFileMonitor(ui->tabWidget);

    ui->tabWidget->addTab(tabFileExplorer, "File Explorer");
    ui->tabWidget->addTab(V2_tabFileMonitor, "V2 - File Monitor");
}

void MainWindow::disableCloseButtonOfPredefinedTabs()
{
    QTabBar *tabBar = ui->tabWidget->tabBar();
    tabBar->tabButton(0, QTabBar::ButtonPosition::RightSide)->deleteLater();
    tabBar->setTabButton(0, QTabBar::ButtonPosition::RightSide, nullptr);

    tabBar->tabButton(1, QTabBar::ButtonPosition::RightSide)->deleteLater();
    tabBar->setTabButton(1, QTabBar::ButtonPosition::RightSide, nullptr);
}

void MainWindow::createV2_FileMonitorThread()
{
    V2_fileMonitorThread = new QThread(this);
    V2_fileMonitorThread->setObjectName(fileMonitorThreadName());

    V2_FileMonitoringManager *monitor = new V2_FileMonitoringManager(DatabaseRegistry::fileSystemEventDatabase());

    QString monitoredPath = QStandardPaths::writableLocation(QStandardPaths::StandardLocation::DesktopLocation);
    monitoredPath.append(QDir::separator());
    monitoredPath.append("data");

//    monitor->setPredictionList({monitoredPath, "/home/user/Desktop/non_exist", "/home/user/Desktop/no_such_file.txt"});

    auto fsm = FileStorageManager::instance();
    auto queryResult = fsm->getMonitoredFilePathList();
    queryResult.append(fsm->getMonitoredFolderPathList());
    monitor->setPredictionList(queryResult);

    monitor->moveToThread(V2_fileMonitorThread);

    QObject::connect(V2_fileMonitorThread, &QThread::started,
                     monitor, &V2_FileMonitoringManager::start);

    QObject::connect(V2_fileMonitorThread, &QThread::finished,
                     monitor, &QObject::deleteLater);

    V2_fileMonitorThread->start();
}

QString MainWindow::fileMonitorThreadName() const
{
    return "File Monitor Thread";
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

void MainWindow::on_tab1Action_AddNewFolder_triggered()
{
    Qt::WindowFlags flags = dialogAddNewFolder->windowFlags();
    flags |= Qt::WindowMaximizeButtonHint;
    dialogAddNewFolder->setWindowFlags(flags);
    dialogAddNewFolder->setModal(true);
    dialogAddNewFolder->show(tabFileExplorer->currentDir());
}

void MainWindow::on_menuAction_DebugFileMonitor_triggered()
{
    dialogDebugFileMonitor->show();
}
