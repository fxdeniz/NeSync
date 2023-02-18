#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "Tabs/TabFileMonitor.h"
#include "Tabs/TabFileExplorer.h"
#include "Dialogs/DialogAddNewFolder.h"
#include "Dialogs/DialogDebugFileMonitor.h"
#include "Backend/FileMonitorSubSystem/FileMonitoringManager.h"

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

private slots:
    void on_tabWidget_currentChanged(int index);
    void on_tab1Action_AddNewFolder_triggered();
    void on_menuAction_DebugFileMonitor_triggered();
    void on_tab2Action_SaveAll_triggered();

private:
    TabFileExplorer *tabFileExplorer;
    TabFileMonitor *tabFileMonitor;
    QAction *separator1, *separator2, *separator3;

private:
    void allocateSeparators();
    void buildTabWidget();
    void disableCloseButtonOfPredefinedTabs();
    void createFileMonitorThread();
    QString fileMonitorThreadName() const;

private:
    Ui::MainWindow *ui;
    DialogAddNewFolder *dialogAddNewFolder;
    DialogDebugFileMonitor *dialogDebugFileMonitor;
    FileMonitoringManager *fmm;
    QThread *fileMonitorThread;

};

#endif // MAINWINDOW_H
