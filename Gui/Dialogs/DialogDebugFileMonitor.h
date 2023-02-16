#ifndef DIALOGDEBUGFILEMONITOR_H
#define DIALOGDEBUGFILEMONITOR_H

#include <QDialog>
#include <QSqlDatabase>

namespace Ui {
class DialogDebugFileMonitor;
}

class DialogDebugFileMonitor : public QDialog
{
    Q_OBJECT

public:
    explicit DialogDebugFileMonitor(QWidget *parent = nullptr);
    ~DialogDebugFileMonitor();

private slots:
    void on_buttonExecute_clicked();

    void on_tabWidget_currentChanged(int index);

private:
    Ui::DialogDebugFileMonitor *ui;
    QSqlDatabase database;
};

#endif // DIALOGDEBUGFILEMONITOR_H
