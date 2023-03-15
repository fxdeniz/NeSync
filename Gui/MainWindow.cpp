#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "Utility/JsonDtoFormat.h"
#include "Backend/FileStorageSubSystem/FileStorageManager.h"

#include <QDir>
#include <QTabBar>
#include <QMessageBox>
#include <QStandardPaths>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QThread::currentThread()->setObjectName(guiThreadName());

    dialogImport = new DialogImport(this);
    dialogAddNewFolder = new DialogAddNewFolder(this);
    dialogDebugFileMonitor = new DialogDebugFileMonitor(this);

    allocateSeparators();
    buildTabWidget();
    disableCloseButtonOfPredefinedTabs();
    on_tabWidget_currentChanged(ui->tabWidget->currentIndex());

    ui->tabWidget->setCurrentIndex(0);

    QObject::connect(dialogImport, &QDialog::accepted,
                     tabFileExplorer, &TabFileExplorer::refreshFileExplorer);

    QObject::connect(dialogAddNewFolder, &QDialog::accepted,
                     tabFileExplorer, &TabFileExplorer::refreshFileExplorer);

    QObject::connect(dialogAddNewFolder, &DialogAddNewFolder::accepted,
                     tabFileMonitor, &TabFileMonitor::onEventDbUpdated);

    QObject::connect(tabFileMonitor, &TabFileMonitor::signalEnableSaveAllButton,
                     ui->tab2Action_SaveAll, &QAction::setEnabled);

    createFileMonitorThread(dialogImport, tabFileExplorer);
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

    if(index == 1)
        toolBar->addAction(ui->tab2Action_SaveAll);
    else if(index == 0)
    {
        toolBar->addAction(ui->tab1Action_AddNewFolder);
        toolBar->addAction(separator1);
        toolBar->addAction(ui->tab1Action_Import);
    }
}

void MainWindow::allocateSeparators()
{
    separator1 = new QAction(this);
    separator1->setSeparator(true);
}

void MainWindow::buildTabWidget()
{
    tabFileExplorer = new TabFileExplorer(ui->tabWidget);
    tabFileMonitor = new TabFileMonitor(ui->tabWidget);

    ui->tabWidget->addTab(tabFileExplorer, tr("File Explorer"));
    ui->tabWidget->addTab(tabFileMonitor, tr("File Monitor"));
}

void MainWindow::disableCloseButtonOfPredefinedTabs()
{
    QTabBar *tabBar = ui->tabWidget->tabBar();
    tabBar->tabButton(0, QTabBar::ButtonPosition::RightSide)->deleteLater();
    tabBar->setTabButton(0, QTabBar::ButtonPosition::RightSide, nullptr);

    tabBar->tabButton(1, QTabBar::ButtonPosition::RightSide)->deleteLater();
    tabBar->setTabButton(1, QTabBar::ButtonPosition::RightSide, nullptr);
}

void MainWindow::createFileMonitorThread(const DialogImport * const dialogImport,
                                         const TabFileExplorer * const tabFileExplorer)
{
    fileMonitorThread = new QThread(this);
    fileMonitorThread->setObjectName(fileMonitorThreadName());

    fmm = new FileMonitoringManager();

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

    QObject::connect(dialogImport, &DialogImport::signalFileImportStartedForActiveFile,
                     fmm, &FileMonitoringManager::stopMonitoringTarget,
                     Qt::ConnectionType::BlockingQueuedConnection);

    QObject::connect(tabFileExplorer, &TabFileExplorer::signalStopFileMonitor,
                     fmm, &FileMonitoringManager::pauseMonitoring,
                     Qt::ConnectionType::BlockingQueuedConnection);

    QObject::connect(tabFileExplorer, &TabFileExplorer::signalStartFileMonitor,
                     fmm, &FileMonitoringManager::continueMonitoring,
                     Qt::ConnectionType::BlockingQueuedConnection);

    QObject::connect(tabFileExplorer, &TabFileExplorer::signalStartMonitoringItem,
                     fmm, &FileMonitoringManager::addTargetAtRuntime,
                     Qt::ConnectionType::BlockingQueuedConnection);

    QObject::connect(tabFileExplorer, &TabFileExplorer::signalStopMonitoringItem,
                     fmm, &FileMonitoringManager::stopMonitoringTarget,
                     Qt::ConnectionType::BlockingQueuedConnection);

    fmm->moveToThread(fileMonitorThread);

    fileMonitorThread->start();
}

QString MainWindow::fileMonitorThreadName() const
{
    return "File Monitor Thread";
}

void MainWindow::on_tab1Action_AddNewFolder_triggered()
{
    Qt::WindowFlags flags = dialogAddNewFolder->windowFlags();
    flags |= Qt::WindowMaximizeButtonHint;
    dialogAddNewFolder->setWindowFlags(flags);
    dialogAddNewFolder->setModal(true);
    dialogAddNewFolder->show(tabFileExplorer->currentSymbolFolderPath(), fmm);
}

void MainWindow::on_tab1Action_Import_triggered()
{
    Qt::WindowFlags flags = dialogImport->windowFlags();
    flags |= Qt::WindowMaximizeButtonHint;
    dialogImport->setWindowFlags(flags);
    dialogImport->setModal(true);
    dialogImport->show();
}

void MainWindow::on_tab2Action_SaveAll_triggered()
{
    tabFileMonitor->saveChanges(fmm);
}

void MainWindow::on_menuAction_DebugFileMonitor_triggered()
{
    dialogDebugFileMonitor->show();
}

void MainWindow::on_menuAction_AboutApp_triggered()
{
    auto fsm = FileStorageManager::instance();

    QString title = tr("About NeSync");
    QString message = tr("<center><h1>NeSync 1.5.0</h1><center/>"
                         "<hr>"
                         "Thanks for using NeSync.<br>"
                         "This is a <b>early access version</b>, consider this as staging period towards V2.<br>"
                         "This software does not collect any data and does not connect to the internet. <br>"
                         ""
                         "<h3>Developed by</h3>"
                         "<b>Deniz YILMAZOK</b> | <a href = \"https://www.github.com/fxdeniz\">fxdeniz (GitHub Profile)</a><br>"
                         ""
                         "<h3>Thanks to</h3>"
                         "<dl>"
                         "<dt>"
                         "  <b>SpartanJ</b> for efsw library | <a href = \"https://www.github.com/SpartanJ/efsw\">efsw (GitHub Repo)</a>"
                         "</dt>"
                         "<dt>"
                         "  <b>stachenov</b> for QuaZip library | <a href = \"https://github.com/stachenov/quazip\">QuaZip (GitHub Repo)</a>"
                         "</dt>"
                         "<dt>"
                         "  <b>Qt Framework developers</b> | <a href = \"https://www.qt.io\">The Qt Company Website</a>"
                         "</dt>"
                         "<dt>"
                         "  <b>SQLite project team</b> | <a href = \"https://www.sqlite.org\"><a/>SQLite Home Page"
                         "</dt>"
                         "</dl>"
                         "<br>"
                         "<center>"
                         "NeSync is a Free and open-source software (FOSS)."
                         "</center>"
                         "<center>"
                         "This software released under <a href =\"https://www.gnu.org/licenses/gpl-3.0.en.html\">GPL Version 3 (gnu.org)</a> license."
                         "</center>"
                         "<br>"
                         "<b>Backup folder path:</b> %1").arg(fsm->getBackupFolderPath());

    QMessageBox::information(this, title, message);
}

void MainWindow::on_menuAction_AboutQt_triggered()
{
    QApplication::aboutQt();
}
