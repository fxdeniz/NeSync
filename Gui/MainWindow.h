#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "Tabs/V2_TabFileMonitor.h"
#include "Dialogs/DialogAddNewFolder.h"
#include "Dialogs/DialogFileOrDirEditor.h"
#include "Dialogs/DialogDebugFileMonitor.h"
#include "Tabs/TabFileExplorer.h"

#define DEBUG_FSM_TO_GUI
#define DEBUG_FSM_TO_GUI_WITHOUT_THREAD_INFO

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
    void on_router_ShowRelatedFiles();
    void on_router_ShowDialogTableItemEditor();
    void on_tab1Action_AddNewFolder_triggered();
    void on_menuAction_DebugFileMonitor_triggered();

private:
    TabFileExplorer *tabFileExplorer;
    V2_TabFileMonitor *V2_tabFileMonitor;
    QAction *separator1, *separator2, *separator3;

private:
    void allocateSeparators();
    void buildTabWidget();
    void disableCloseButtonOfPredefinedTabs();
    void createV2_FileMonitorThread();
    QString fileMonitorThreadName() const;

private:
    Ui::MainWindow *ui;
    DialogFileOrDirEditor *dialogTableItemEditor;
    DialogAddNewFolder *dialogAddNewFolder;
    DialogDebugFileMonitor *dialogDebugFileMonitor;
    QThread *V2_fileMonitorThread;

};

#endif // MAINWINDOW_H
