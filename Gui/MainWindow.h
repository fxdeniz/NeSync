#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "UserContentTableModel.h"
#include "UserContentListModel.h"
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

    void on_buttonSelectControl_clicked();
    void on_actionEditTableItem_clicked();

private:
    Ui::MainWindow *ui;
    DialogTableItemEditor *dialogTableItemEditor;
    UserContentTableModel *userContentTableModel;
    UserContentListModel *userContentListModel;
};

#endif // MAINWINDOW_H
