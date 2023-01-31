#include "TabFileMonitor.h"
#include "ui_TabFileMonitor.h"

#include "DataModels/TabFileMonitor/TreeModelFsMonitor.h"

TabFileMonitor::TabFileMonitor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabFileMonitor)
{
    ui->setupUi(this);
    itemDelegateAction = new ItemDelegateAction(this);
}

TabFileMonitor::~TabFileMonitor()
{
    delete ui;
}

void TabFileMonitor::onEventDbUpdated()
{
    TreeModelFsMonitor *newModel = new TreeModelFsMonitor();
    QAbstractItemModel *oldModel = ui->treeView->model();

    if(oldModel != nullptr)
        delete oldModel;

    ui->treeView->setModel(newModel);

    QHeaderView *header = ui->treeView->header();
    header->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
    header->setSectionResizeMode(TreeModelFsMonitor::ColumnIndexUserPath, QHeaderView::ResizeMode::Interactive);
    header->setMinimumSectionSize(120);
    ui->treeView->setColumnWidth(TreeModelFsMonitor::ColumnIndexUserPath, 500);

    ui->treeView->setItemDelegateForColumn(TreeModelFsMonitor::ColumnIndexAction, itemDelegateAction);

    ui->treeView->expandAll();

    ui->treeView->setSelectionMode(QAbstractItemView::SelectionMode::ContiguousSelection);
    ui->treeView->selectAll();
    QModelIndexList selectedIndices = ui->treeView->selectionModel()->selectedIndexes();

    for(const QModelIndex &current : selectedIndices)
        ui->treeView->openPersistentEditor(current.siblingAtColumn(TreeModelFsMonitor::ColumnIndexAction));

    ui->treeView->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
    ui->treeView->selectionModel()->clearSelection();
}
