#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "Utility/JsonDtoFormat.h"
#include "Utility/DatabaseRegistry.h"
#include "Backend/FileStorageSubSystem/FileStorageManager.h"

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

    fmm = new FileMonitoringManager(DatabaseRegistry::fileSystemEventDatabase());

    auto fsm = FileStorageManager::instance();

    QJsonArray activeFolders = fsm->getActiveFolderList();
    QJsonArray activeFiles = fsm->getActiveFileList();
    QStringList predictionList;

    for(const QJsonValue &value : activeFolders)
        predictionList << value.toObject()[JsonKeys::Folder::UserFolderPath].toString();

    for(const QJsonValue &value : activeFiles)
        predictionList << value.toObject()[JsonKeys::File::UserFilePath].toString();

    fmm->setPredictionList(predictionList);

    QObject::connect(fmm, &FileMonitoringManager::signalEventDbUpdated,
                     tabFileMonitor, &TabFileMonitor::onEventDbUpdated);

    QObject::connect(fileMonitorThread, &QThread::started,
                     fmm, &FileMonitoringManager::start);

    QObject::connect(fileMonitorThread, &QThread::finished,
                     fmm, &QObject::deleteLater);

    fmm->moveToThread(fileMonitorThread);

    fileMonitorThread->start();
}

QString MainWindow::fileMonitorThreadName() const
{
    return "File Monitor Thread";
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

void MainWindow::on_tab2Action_SaveAll_triggered()
{
    tabFileMonitor->saveChanges(fmm);
}

