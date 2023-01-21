#include "V2_TabFileMonitor.h"
#include "ui_V2_TabFileMonitor.h"

V2_TabFileMonitor::V2_TabFileMonitor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::V2_TabFileMonitor)
{
    ui->setupUi(this);
}

V2_TabFileMonitor::~V2_TabFileMonitor()
{
    delete ui;
}
