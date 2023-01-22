#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "Tabs/TabFileMonitor.h"
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
    TabFileMonitor *tabFileMonitor;
    V2_TabFileMonitor *V2_tabFileMonitor;
    QAction *separator1, *separator2, *separator3;

private:
    void allocateSeparators();
    void buildTabWidget();
    void disableCloseButtonOfPredefinedTabs();
    void createFileMonitorThread();
    void createV2_FileMonitorThread();
    QString fileMonitorThreadName() const;

private:
    Ui::MainWindow *ui;
    DialogFileOrDirEditor *dialogTableItemEditor;
    DialogAddNewFolder *dialogAddNewFolder;
    DialogDebugFileMonitor *dialogDebugFileMonitor;
    QThread *fileMonitorThread;
    QThread *V2_fileMonitorThread;

#ifdef DEBUG_FSM_TO_GUI
private slots:
    void slotOnPredictionTargetNotFound(const QString &pathToTaget);
    void slotOnUnPredictedFolderDetected(const QString &pathToFolder);
    void slotOnUnPredictedFileDetected(const QString &pathToFile);
    void slotOnFileSystemEventAnalysisStarted();
    void slotOnNewFolderAdded(const QString &pathToFolder);
    void slotOnFolderDeleted(const QString &pathToFolder);
    void slotOnFolderMoved(const QString &pathToFolder, const QString &oldFolderName);
    void slotOnNewFileAdded(const QString &pathToFile);
    void slotOnFileDeleted(const QString &pathToFile);
    void slotOnFileModified(const QString &pathToFile);
    void slotOnFileMoved(const QString &pathToFile, const QString &oldFileName);
    void slotOnFileMovedAndModified(const QString &pathToFile, const QString &oldFileName);
#endif

};

#endif // MAINWINDOW_H
