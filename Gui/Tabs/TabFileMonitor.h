#ifndef TABFILEMONITOR_H
#define TABFILEMONITOR_H

#include <QWidget>

namespace Ui {
class TabFileMonitor;
}

class TabFileMonitor : public QWidget
{
    Q_OBJECT

public:
    explicit TabFileMonitor(QWidget *parent = nullptr);
    ~TabFileMonitor();

private:
    Ui::TabFileMonitor *ui;
};

#endif // TABFILEMONITOR_H
