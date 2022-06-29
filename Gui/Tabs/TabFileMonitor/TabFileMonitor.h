#ifndef TABFILEMONITOR_H
#define TABFILEMONITOR_H

#include <QWidget>

#include "DataModels/ItemDelegates/ComboBoxItemDelegateFileAction.h"
#include "DataModels/ItemDelegates/ComboBoxItemDelegateNote.h"
#include "DataModels/TableModelFileMonitor.h"

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
    TableModelFileMonitor *tableModelFileMonitor;
    ComboBoxItemDelegateNote *comboBoxItemDelegateNote;
    ComboBoxItemDelegateFileAction *comboBoxItemDelegateFileAction;

};

#endif // TABFILEMONITOR_H
