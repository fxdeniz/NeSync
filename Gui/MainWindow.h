#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "Tabs/TabFileMonitor.h"
#include "Dialogs/V2_DialogAddNewFolder.h"
#include "Dialogs/DialogFileOrDirEditor.h"
#include "Dialogs/DialogAddNewFolder.h"
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
    void on_tab1Action_NewFolder_triggered();

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
    DialogFileOrDirEditor *dialogTableItemEditor;
    DialogAddNewFolder *dialogAddNewFolder;
    V2_DialogAddNewFolder *v2_dialogAddNewFolder;
    QThread *fileMonitorThread;

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

    void on_tab1Action_v2_NewFolder_triggered();
};

#endif // MAINWINDOW_H
