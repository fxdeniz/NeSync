#include "DialogDebugFileMonitor.h"
#include "ui_DialogDebugFileMonitor.h"

DialogDebugFileMonitor::DialogDebugFileMonitor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogDebugFileMonitor)
{
    ui->setupUi(this);
}

DialogDebugFileMonitor::~DialogDebugFileMonitor()
{
    delete ui;
}
