#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "Utility/AppConfig.h"
#include "Utility/JsonDtoFormat.h"
#include "Backend/FileStorageSubSystem/FileStorageManager.h"

#include <QDir>
#include <QTabBar>
#include <QSettings>
#include <QCloseEvent>
#include <QMessageBox>
#include <QStandardPaths>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QThread::currentThread()->setObjectName(guiThreadName());

    dialogSettings = new DialogSettings(this);
    dialogImport = new DialogImport(this);
    dialogAddNewFolder = new DialogAddNewFolder(this);
    dialogDebugFileMonitor = new DialogDebugFileMonitor(this);

    buildTabWidget();
    createTrayIcon();
    disableCloseButtonOfPredefinedTabs();
    on_tabWidget_currentChanged(ui->tabWidget->currentIndex());

    ui->tabWidget->setCurrentIndex(0);

    QObject::connect(dialogImport, &QDialog::accepted,
                     tabFileExplorer, &TabFileExplorer::refreshFileExplorer);

    QObject::connect(dialogAddNewFolder, &QDialog::accepted,
                     tabFileExplorer, &TabFileExplorer::refreshFileExplorer);

    QObject::connect(dialogAddNewFolder, &DialogAddNewFolder::accepted,
                     tabFileMonitor, &TabFileMonitor::onEventDbUpdated);

    QObject::connect(tabFileExplorer, &TabFileExplorer::signalRefreshFileMonitor,
                     tabFileMonitor, &TabFileMonitor::onEventDbUpdated);

    QObject::connect(tabFileMonitor, &TabFileMonitor::signalFileMonitorRefreshed,
                     this, &MainWindow::onNotificationRequested);

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

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!event->spontaneous() || !isVisible())
        return;

    AppConfig config;

    if (trayIcon->isVisible() && !config.isTrayIconInformed())
    {
        config.setTrayIconInformed(true);

        QMessageBox::information(this, tr("Running in the background"),
                                 tr("NeSync windows will be minimized to system tray.<br>"
                                    "However, your folders still be monitored in the background.<br>"
                                    "To terminate the NeSync, choose <b>Quit</b> in the context menu of the system tray entry."));
        hide();
        event->ignore();
    }
}

void MainWindow::onTrayIconClicked(QSystemTrayIcon::ActivationReason reason)
{
    if(reason == QSystemTrayIcon::ActivationReason::Context)
        return;

    show();
}

void MainWindow::onNotificationRequested()
{
    if(isVisible())
        qApp->beep();
    else
    {
        QString title = tr("Activity detected on your folders !");
        QString message = tr("Click here to display file monitor.");

        trayIcon->showMessage(title, message);
        return;
    }
}

void MainWindow::onTrayIconMessageClicked()
{
    ui->tabWidget->setCurrentIndex(TabIndexMonitor);
    show();
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    QToolBar *toolBar = ui->toolBar;

    for(const QAction *action : toolBar->actions())
    {
        if(action->isSeparator())
            delete action;
    }

    toolBar->clear();

    if(index == TabIndexMonitor)
        toolBar->addAction(ui->tab2Action_SaveAll);
    else if(index == TabIndexExplorer)
    {
        toolBar->addAction(ui->tab1Action_AddNewFolder);
        toolBar->addSeparator();
        toolBar->addAction(ui->tab1Action_Import);
    }
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
    tabBar->tabButton(TabIndexExplorer, QTabBar::ButtonPosition::RightSide)->deleteLater();
    tabBar->setTabButton(TabIndexExplorer, QTabBar::ButtonPosition::RightSide, nullptr);

    tabBar->tabButton(TabIndexMonitor, QTabBar::ButtonPosition::RightSide)->deleteLater();
    tabBar->setTabButton(TabIndexMonitor, QTabBar::ButtonPosition::RightSide, nullptr);
}

void MainWindow::createTrayIcon()
{
    QAction *showAction = new QAction(tr("&Show"), this);
    QObject::connect(showAction, &QAction::triggered, this, &QWidget::showNormal);

    QAction *quitAction = new QAction(tr("&Quit"), this);
    QObject::connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);

    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(showAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    QObject::connect(trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::onTrayIconClicked);
    QObject::connect(trayIcon, &QSystemTrayIcon::messageClicked, this, &MainWindow::onTrayIconMessageClicked);
    trayIcon->setContextMenu(trayIconMenu);

    QIcon icon(":/Resources/tray_icon.png");
    trayIcon->setIcon(icon);

    trayIcon->show();
    trayIcon->setToolTip("NeSync");
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

void MainWindow::on_menuAction_Settings_triggered()
{
    dialogSettings->setModal(true);
    dialogSettings->show();
}

void MainWindow::on_menuAction_DebugFileMonitor_triggered()
{
    dialogDebugFileMonitor->show();
}

void MainWindow::on_menuAction_AboutApp_triggered()
{
    QString title = tr("About NeSync");
    QString message = tr("<center><h1>NeSync 1.9.0 [Pre-Alpha]</h1><center/>"
                         "<hr>"
                         "Thanks for using NeSync.<br>"
                         "This is a <b>pre-alpha version</b>, <b>DO NOT USE</b> for critical things.<br>"
                         "This software does not collect any data and does not connect to the internet. <br>"
                         ""
                         "<h3>Developed by</h3>"
                         "<b>Deniz Yilmazok</b> | <a href = \"https://www.github.com/fxdeniz\">fxdeniz (GitHub Profile)</a><br>"
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
                         "<dt>"
                         "  <b>Jean-loup Gailly</b> & <b>Mark Adler</b> for zlib library | <a href = \"https://zlib.net\">zlib Home Site</a>"
                         "</dt>"
                         "<dt>"
                         " <b>Julian Seward</b> for bzip2 library | <a href = \"https://sourceware.org/bzip2\">bzip2 Home</a>"
                         "</dt>"
                         "<dt>"
                         "  <b>juicy_fish</b> for application icon | <a href = \"https://www.freepik.com/icon/version-control_5815495\">freepik.com</a>"
                         "</dt>"
                         "<dt>"
                         "  <b>NSIS Community</b> for Windows installer | <a href = \"https://nsis.sourceforge.io\">NSIS Wiki</a>"
                         "</dt>"
                         "</dl>"
                         "<br>"
                         "<center>"
                         "NeSync is a Free and open-source software (FOSS)."
                         "</center>"
                         ""
                         "<center>"
                         "This software distributed under <a href =\"https://www.gnu.org/licenses/gpl-3.0.en.html\">GPL Version 3 (gnu.org)</a> license."
                         "</center>"
                         ""
                         "<center>"
                         "Source code is also <b>open to the public</b> under the GPL Version 3 license."
                         "</center>"
                         ""
                         "<center>"
                         "Source code is available in this <a href=\"https://github.com/fxdeniz/NeSync\">GitHub Repo</a>."
                         "</center>"
                        );

    QMessageBox::information(this, title, message);
}

void MainWindow::on_menuAction_AboutQt_triggered()
{
    QApplication::aboutQt();
}
