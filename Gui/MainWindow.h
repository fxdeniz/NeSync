#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "ComboBoxItemDelegateFileAction.h"
#include "ComboBoxItemDelegateNote.h"
#include "TableModelFileExplorer.h"
#include "TableModelFileMonitor.h"
#include "ListModelFileExplorer.h"
#include "DialogTableItemEditor.h"

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
    void showContextMenuListView(const QPoint &argPos);
    void showContextMenuTableView(const QPoint &argPos);

    void on_actionEditTableItem_clicked();

    void on_tabWidget_currentChanged(int index);

private:
    QMenu *contextMenuTableFileExplorer;
    QMenu *contextMenuListFileExplorer;
    QAction *separator1, *separator2, *separator3;

private:
    void allocateSeparators();
    void buildContextMenuTableFileExplorer();
    void buildContextMenuListFileExplorer();

private:
    Ui::MainWindow *ui;
    DialogTableItemEditor *dialogTableItemEditor;
    TableModelFileExplorer *tableModelFileExplorer;
    ListModelFileExplorer *listModelFileExplorer;
    TableModelFileMonitor *tableModelFileMonitor;
    ComboBoxItemDelegateNote *comboBoxItemDelegateNote;
    ComboBoxItemDelegateFileAction *comboBoxItemDelegateFileAction;
};

#endif // MAINWINDOW_H
