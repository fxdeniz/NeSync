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

    void saveChanges();

public slots:
    void onEventDbUpdated();

private slots:
    void on_buttonAddDescription_clicked();
    void on_buttonDeleteDescription_clicked();
    void on_textEditDescription_textChanged();

    void on_comboBoxDescriptionNumber_activated(int index);

private:
    Ui::TabFileMonitor *ui;
    ItemDelegateAction *itemDelegateAction;
    ItemDelegateDescription *itemDelegateDescription;
};

#endif // TABFILEMONITOR_H
