#include "TabFileMonitor.h"
#include "ui_TabFileMonitor.h"

#include "Utility/DatabaseRegistry.h"
#include "DataModels/TabFileMonitor/TreeModelFsMonitor.h"

TabFileMonitor::TabFileMonitor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabFileMonitor)
{
    ui->setupUi(this);
}

TabFileMonitor::~TabFileMonitor()
{
    delete ui;
}

void TabFileMonitor::onEventDbUpdated()
{
    TreeModelFsMonitor *newModel = new TreeModelFsMonitor(DatabaseRegistry::fileSystemEventDatabase());
    QAbstractItemModel *oldModel = ui->treeView->model();

    if(oldModel != nullptr)
        delete oldModel;

    ui->treeView->setModel(newModel);
}
