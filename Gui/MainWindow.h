#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "Backend/FileStorageSubSystem/FileStorageManager.h"
#include "Dialogs/DialogFileOrDirEditor.h"
#include "Dialogs/DialogAddNewFile.h"
#include "Tabs/TabFileExplorer.h"
#include "Tabs/TabFileMonitor.h"

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

private slots:
    void on_tabWidget_currentChanged(int index);

    void on_router_ShowRelatedFiles();
    void on_router_ShowDialogTableItemEditor();

    void on_tab1Action_AddFile_triggered();

private:
    TabFileExplorer *tabFileExplorer;
    TabFileMonitor *tabFileMonitor;
    QAction *separator1, *separator2, *separator3;

private:
    void allocateSeparators();
    void buildTabWidget();
    void disableCloseButtonOfPredefinedTabs();

private:
    Ui::MainWindow *ui;
    DialogFileOrDirEditor *dialogTableItemEditor;
    DialogAddNewFile *dialogAddNewFile;
    FileStorageManager *fileStorageManager;
};

#endif // MAINWINDOW_H
