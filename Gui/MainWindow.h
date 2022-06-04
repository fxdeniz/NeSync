#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "UserContentTableModel.h"
#include "UserContentListModel.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    UserContentTableModel *userContentTableModel;
    UserContentListModel *userContentListModel;
};

#endif // MAINWINDOW_H
