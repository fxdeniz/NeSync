#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "Tabs/TabRelatedFiles.h"

#include <QStandardPaths>
#include <QtConcurrent>
#include <QDir>

#include "Backend/FileMonitorSubSystem/FileMonitoringManagerIntegrationTest.h"

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

    QObject::connect(dialogAddNewFile, &QDialog::accepted,
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

void MainWindow::createFileMonitorThread()
{
    auto queryResult = FileStorageManager::instance()->getMonitoredFilePathList();
    queryResult << "not_exist.txt";
    //new FileMonitoringManagerIntegrationTest(list);
    fileMonitorThread = new QThread(this);
    fileMonitorThread->setObjectName(fileMonitorThreadName());

    QTimer *timer = new QTimer();
    FileMonitoringManager *monitor = new FileMonitoringManager(timer);
    monitor->setPredictionList(queryResult);

    timer->moveToThread(fileMonitorThread);
    monitor->moveToThread(fileMonitorThread);

    QObject::connect(fileMonitorThread, &QThread::started,
                     monitor, &FileMonitoringManager::start);

    QObject::connect(fileMonitorThread, &QThread::finished,
                     timer, &QObject::deleteLater);

    QObject::connect(fileMonitorThread, &QThread::finished,
                     monitor, &QObject::deleteLater);

//    QObject::connect(monitor, &FileMonitoringManager::signalPredictedFileNotFound,
//                     this, &MainWindow::slotOnPredictedFileNotFound);

    QObject::connect(monitor, &FileMonitoringManager::signalPredictionTargetNotFound,
                     tabFileMonitor, &TabFileMonitor::slotOnPredictionTargetNotFound);

    QObject::connect(monitor, &FileMonitoringManager::signalUnPredictedFolderDetected,
                     tabFileMonitor, &TabFileMonitor::slotOnUnPredictedFolderDetected);

    QObject::connect(monitor, &FileMonitoringManager::signalUnPredictedFileDetected,
                     tabFileMonitor, &TabFileMonitor::slotOnUnPredictedFileDetected);

    QObject::connect(monitor, &FileMonitoringManager::signalNewFolderAdded,
                     tabFileMonitor, &TabFileMonitor::slotOnNewFolderAdded);

    QObject::connect(monitor, &FileMonitoringManager::signalNewFileAdded,
                     tabFileMonitor, &TabFileMonitor::slotOnNewFileAdded);

    QObject::connect(monitor, &FileMonitoringManager::signalFolderDeleted,
                     tabFileMonitor, &TabFileMonitor::slotOnFolderDeleted);




    QObject::connect(monitor, &FileMonitoringManager::signalPredictionTargetNotFound,
                     this, &MainWindow::slotOnPredictionTargetNotFound);

    QObject::connect(monitor, &FileMonitoringManager::signalUnPredictedFolderDetected,
                     this, &MainWindow::slotOnUnPredictedFolderDetected);

    QObject::connect(monitor, &FileMonitoringManager::signalUnPredictedFileDetected,
                     this, &MainWindow::slotOnUnPredictedFileDetected);

    QObject::connect(monitor, &FileMonitoringManager::signalFileSystemEventAnalysisStarted,
                     this, &MainWindow::slotOnFileSystemEventAnalysisStarted);

    QObject::connect(monitor, &FileMonitoringManager::signalNewFolderAdded,
                     this, &MainWindow::slotOnNewFolderAdded);

    QObject::connect(monitor, &FileMonitoringManager::signalFolderDeleted,
                     this, &MainWindow::slotOnFolderDeleted);

    QObject::connect(monitor, &FileMonitoringManager::signalFolderMoved,
                     this, &MainWindow::slotOnFolderMoved);

    QObject::connect(monitor, &FileMonitoringManager::signalNewFileAdded,
                     this, &MainWindow::slotOnNewFileAdded);

    QObject::connect(monitor, &FileMonitoringManager::signalFileDeleted,
                     this, &MainWindow::slotOnFileDeleted);

    QObject::connect(monitor, &FileMonitoringManager::signalFileModified,
                     this, &MainWindow::slotOnFileModified);

    QObject::connect(monitor, &FileMonitoringManager::signalFileMoved,
                     this, &MainWindow::slotOnFileMoved);

    QObject::connect(monitor, &FileMonitoringManager::signalFileMovedAndModified,
                     this, &MainWindow::slotOnFileMovedAndModified);

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

void MainWindow::slotOnPredictionTargetNotFound(const QString &pathToTaget)
{
    qDebug() << "slotOnPredictionTargetNotFound = " << pathToTaget << " in " << QThread::currentThread();
    qDebug() << "";
}

void MainWindow::slotOnUnPredictedFolderDetected(const QString &pathToFolder)
{
    qDebug() << "slotOnUnPredictedFolderDetected = " << pathToFolder << " in " << QThread::currentThread();
    qDebug() << "";
}

void MainWindow::slotOnUnPredictedFileDetected(const QString &pathToFile)
{
    qDebug() << "slotOnUnPredictedFileDetected = " << pathToFile << " in " << QThread::currentThread();
    qDebug() << "";
}

void MainWindow::slotOnFileSystemEventAnalysisStarted()
{
    qDebug() << "slotOnFileSystemEventAnalysisStarted(void)" << " in " << QThread::currentThread();
    qDebug() << "";
}

void MainWindow::slotOnNewFolderAdded(const QString &pathToFolder)
{
    qDebug() << "slotOnNewFolderAdded = " << pathToFolder << " in " << QThread::currentThread();
    qDebug() << "";
}

void MainWindow::slotOnFolderDeleted(const QString &pathToFolder)
{
    qDebug() << "slotOnFolderDeleted = " << pathToFolder << " in " << QThread::currentThread();
    qDebug() << "";
}

void MainWindow::slotOnFolderMoved(const QString &pathToFolder, const QString &oldFolderName)
{
    qDebug() << "slotOnFolderMoved()    pathToFolder = " << pathToFolder << "    oldFolderName = " << oldFolderName << " in " << QThread::currentThread();
    qDebug() << "";
}

void MainWindow::slotOnNewFileAdded(const QString &pathToFile)
{
    qDebug() << "slotOnNewFileAdded = " << pathToFile << " in " << QThread::currentThread();
    qDebug() << "";
}

void MainWindow::slotOnFileDeleted(const QString &pathToFile)
{
    qDebug() << "slotOnFileDeleted = " << pathToFile << " in " << QThread::currentThread();
    qDebug() << "";
}

void MainWindow::slotOnFileModified(const QString &pathToFile)
{
    qDebug() << "slotOnFileModified = " << pathToFile << " in " << QThread::currentThread();
    qDebug() << "";
}

void MainWindow::slotOnFileMoved(const QString &pathToFile, const QString &oldFileName)
{
    qDebug() << "slotOnFileMoved()    pathToFile = " << pathToFile << "    oldFileName = " << oldFileName << " in " << QThread::currentThread();
    qDebug() << "";
}

void MainWindow::slotOnFileMovedAndModified(const QString &pathToFile, const QString &oldFileName)
{
    qDebug() << "slotOnFileMovedAndModified()    pathToFile = " << pathToFile << "    oldFileName = " << oldFileName << " in " << QThread::currentThread();
    qDebug() << "";
}
