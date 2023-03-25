#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Tabs/TabFileMonitor.h"
#include "Tabs/TabFileExplorer.h"
#include "Dialogs/DialogImport.h"
#include "Dialogs/DialogAddNewFolder.h"
#include "Dialogs/DialogDebugFileMonitor.h"
#include "Backend/FileMonitorSubSystem/FileMonitoringManager.h"

#include <QThread>
#include <QMainWindow>
#include <QSystemTrayIcon>

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QString guiThreadName() const;

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void trayIconClicked(QSystemTrayIcon::ActivationReason reason);
    void on_tabWidget_currentChanged(int index);
    void on_tab1Action_AddNewFolder_triggered();
    void on_tab1Action_Import_triggered();
    void on_tab2Action_SaveAll_triggered();
    void on_menuAction_DebugFileMonitor_triggered();
    void on_menuAction_AboutApp_triggered();
    void on_menuAction_AboutQt_triggered();

private:
    TabFileExplorer *tabFileExplorer;
    TabFileMonitor *tabFileMonitor;
    QAction *separator1;

private:
    void showLiabilityWarningInStatusBar();
    void allocateSeparators();
    void buildTabWidget();
    void disableCloseButtonOfPredefinedTabs();
    void createTrayIcon();
    void createFileMonitorThread(const DialogImport * const dialogImport,
                                 const TabFileExplorer * const tabFileExplorer);
    QString fileMonitorThreadName() const;

private:
    Ui::MainWindow *ui;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

    QAction *minimizeAction;
    QAction *maximizeAction;
    QAction *restoreAction;
    QAction *quitAction;

    DialogAddNewFolder *dialogAddNewFolder;
    DialogImport *dialogImport;
    DialogDebugFileMonitor *dialogDebugFileMonitor;
    FileMonitoringManager *fmm;
    QThread *fileMonitorThread;

};

#endif // MAINWINDOW_H
