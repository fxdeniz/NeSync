#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "Tabs/TabRelatedFiles.h"
#include "Utility/JsonDtoFormat.h"
#include "Utility/DatabaseRegistry.h"
#include "Backend/FileStorageSubSystem/V2_FileStorageManager.h"
#include "Backend/FileMonitorSubSystem/FileMonitoringManager.h"

#include <QStandardPaths>
#include <QtConcurrent>
#include <QTabBar>
#include <QDir>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QThread::currentThread()->setObjectName(guiThreadName());

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

    createFileMonitorThread();
}

MainWindow::~MainWindow()
{
    fileMonitorThread->quit();
    fileMonitorThread->wait();

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

void MainWindow::createFileMonitorThread()
{
    fileMonitorThread = new QThread(this);
    fileMonitorThread->setObjectName(fileMonitorThreadName());

    FileMonitoringManager *monitor = new FileMonitoringManager(DatabaseRegistry::fileSystemEventDatabase());

    auto fsm = V2_FileStorageManager::instance();

    QJsonArray activeFolders = fsm->getActiveFolderList();
    QJsonArray activeFiles = fsm->getActiveFileList();
    QStringList predictionList;

    for(const QJsonValue &value : activeFolders)
        predictionList << value.toObject()[JsonKeys::Folder::UserFolderPath].toString();

    for(const QJsonValue &value : activeFiles)
        predictionList << value.toObject()[JsonKeys::File::UserFilePath].toString();

    monitor->setPredictionList(predictionList);

    monitor->moveToThread(fileMonitorThread);

    QObject::connect(fileMonitorThread, &QThread::started,
                     monitor, &FileMonitoringManager::start);

    QObject::connect(fileMonitorThread, &QThread::finished,
                     monitor, &QObject::deleteLater);

    fileMonitorThread->start();
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
