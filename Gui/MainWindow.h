#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "ComboBoxItemDelegateFileAction.h"
#include "ComboBoxItemDelegateNote.h"
#include "TableModelFileMonitor.h"
#include "DialogTableItemEditor.h"
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

    void on_contextActionListFileExplorer_ShowRelatedFiles_triggered();

private:
    TabFileExplorer *tabFileExplorer;
    QAction *separator1, *separator2, *separator3;

private:
    void allocateSeparators();
    void buildTabWidget();
    void disableCloseButtonOfPredefinedTabs();

private:
    Ui::MainWindow *ui;
    DialogTableItemEditor *dialogTableItemEditor;
    TableModelFileMonitor *tableModelFileMonitor;
    ComboBoxItemDelegateNote *comboBoxItemDelegateNote;
    ComboBoxItemDelegateFileAction *comboBoxItemDelegateFileAction;
};

#endif // MAINWINDOW_H
