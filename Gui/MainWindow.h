#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "DialogTableItemEditor.h"
#include "Tabs/TabFileMonitor/TabFileMonitor.h"
#include "TabFileExplorer.h"

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
    DialogTableItemEditor *dialogTableItemEditor;
};

#endif // MAINWINDOW_H
