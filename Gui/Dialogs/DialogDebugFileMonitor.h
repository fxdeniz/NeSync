#ifndef DIALOGDEBUGFILEMONITOR_H
#define DIALOGDEBUGFILEMONITOR_H

#include <QDialog>

namespace Ui {
class DialogDebugFileMonitor;
}

class DialogDebugFileMonitor : public QDialog
{
    Q_OBJECT

public:
    explicit DialogDebugFileMonitor(QWidget *parent = nullptr);
    ~DialogDebugFileMonitor();

private:
    Ui::DialogDebugFileMonitor *ui;
};

#endif // DIALOGDEBUGFILEMONITOR_H
