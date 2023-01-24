#include "TabFileMonitor.h"
#include "ui_TabFileMonitor.h"

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
