#ifndef TABFILEMONITOR_H
#define TABFILEMONITOR_H

#include "DataModels/TabFileMonitor/ItemDelegateAction.h"
#include "DataModels/TabFileMonitor/ItemDelegateDescription.h"

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

public slots:
    void onEventDbUpdated();

private:
    Ui::TabFileMonitor *ui;
    ItemDelegateAction *itemDelegateAction;
    ItemDelegateDescription *itemDelegateDescription;
};

#endif // TABFILEMONITOR_H
