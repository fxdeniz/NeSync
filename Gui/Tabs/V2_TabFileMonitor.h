#ifndef V2_TABFILEMONITOR_H
#define V2_TABFILEMONITOR_H

#include <QWidget>

namespace Ui {
class V2_TabFileMonitor;
}

class V2_TabFileMonitor : public QWidget
{
    Q_OBJECT

public:
    explicit V2_TabFileMonitor(QWidget *parent = nullptr);
    ~V2_TabFileMonitor();

private:
    Ui::V2_TabFileMonitor *ui;
};

#endif // V2_TABFILEMONITOR_H
