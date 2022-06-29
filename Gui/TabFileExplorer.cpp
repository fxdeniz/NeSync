#include "TabFileExplorer.h"
#include "ui_TabFileExplorer.h"

TabFileExplorer::TabFileExplorer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabFileExplorer)
{
    ui->setupUi(this);
    this->buildContextMenuTableFileExplorer();
    this->buildContextMenuListFileExplorer();


    QList<TableModelFileExplorer::TableItem> sampleFileExplorerTableData;

    sampleFileExplorerTableData.insert(0, {"first_file", ".txt"});
    sampleFileExplorerTableData.insert(1, {"second_file", ".zip"});
    sampleFileExplorerTableData.insert(2, {"third_file", ".pdf"});
    sampleFileExplorerTableData.insert(3, {"fourth_file", ".mp4"});

    this->tableModelFileExplorer = new TableModelFileExplorer(sampleFileExplorerTableData, this);
    this->ui->tableViewFileExplorer->setModel(this->tableModelFileExplorer);

    QStringList sampleListData;
    sampleListData << "item 1" << "item 2" << "item 3" << "item 4" << "item 5";

    this->listModelFileExplorer = new ListModelFileExplorer(sampleListData, this);
    this->ui->listView->setModel(this->listModelFileExplorer);

    QObject::connect(this->ui->listView, &QListView::customContextMenuRequested, this, &TabFileExplorer::showContextMenuListView);
    QObject::connect(this->ui->tableViewFileExplorer, &QTableView::customContextMenuRequested, this, &TabFileExplorer::showContextMenuTableView);
}

void TabFileExplorer::buildContextMenuTableFileExplorer()
{
    this->contextMenuTableFileExplorer = new QMenu(this->ui->tableViewFileExplorer);
    QMenu *ptrMenu = this->contextMenuTableFileExplorer;

    QAction *actionOpenFolderInNewTab = this->ui->contextActionTableFileExplorer_OpenFolderInNewTab;
    QAction *actionEdit = this->ui->contextActionTableFileExplorer_Edit;
    QAction *actionCut = this->ui->contextActionTableFileExplorer_Cut;
    QAction *actionFreeze = this->ui->contextActionTableFileExplorer_Freeze;
    QAction *actionDelete = this->ui->contextActionTableFileExplorer_Delete;

    ptrMenu->addAction(actionOpenFolderInNewTab);
    ptrMenu->addAction(actionEdit);
    ptrMenu->addAction(actionCut);
    ptrMenu->addAction(actionFreeze);
    ptrMenu->addAction(actionDelete);

    QObject::connect(actionEdit, &QAction::triggered, this, &TabFileExplorer::on_actionEditTableItem_clicked);
}


void TabFileExplorer::on_actionEditTableItem_clicked()
{
    // TODO move this to router
    //this->dialogTableItemEditor->setModal(true);
    //this->dialogTableItemEditor->show();
}

void TabFileExplorer::buildContextMenuListFileExplorer()
{
    this->contextMenuListFileExplorer = new QMenu(this->ui->listView);
    QMenu *ptrMenu = this->contextMenuListFileExplorer;

    QAction *actionPreview = this->ui->contextActionListFileExplorer_Preview;
    QAction *actionEditVersion = this->ui->contextActionListFileExplorer_EditVersion;
    QAction *actionShowRelatedFiles = this->ui->contextActionListFileExplorer_ShowRelatedFiles;
    QAction *actionSchedule = this->ui->contextActionListFileExplorer_Schedule;
    QAction *actionScheduleAndOpenClipboard = this->ui->contextActionListFileExplorer_ScheduleAndOpenClipboard;
    QAction *actionSetAsCurrentVerion = this->ui->contextActionListFileExplorer_SetAsCurrentVersion;
    QAction *actionDelete = this->ui->contextActionListFileExplorer_Delete;

    ptrMenu->addAction(actionPreview);
    ptrMenu->addAction(actionEditVersion);
    ptrMenu->addAction(actionShowRelatedFiles);
    ptrMenu->addAction(actionSchedule);
    ptrMenu->addAction(actionScheduleAndOpenClipboard);
    ptrMenu->addAction(actionSetAsCurrentVerion);
    ptrMenu->addAction(actionDelete);
}


void TabFileExplorer::showContextMenuTableView(const QPoint &argPos)
{
    QAbstractItemView *subjectView = this->ui->tableViewFileExplorer;
    QModelIndex index = subjectView->indexAt(argPos);

    if(index.isValid()) // If user selected an item from table.
    {
        QMenu *ptrMenu = this->contextMenuTableFileExplorer;
        ptrMenu->popup(subjectView->viewport()->mapToGlobal(argPos));
    }
}

void TabFileExplorer::showContextMenuListView(const QPoint &argPos)
{
    QAbstractItemView *subjectView = this->ui->listView;
    QModelIndex index = subjectView->indexAt(argPos);

    if(index.isValid()) // If user selected an item from list.
    {
        QMenu *ptrMenu = this->contextMenuListFileExplorer;
        ptrMenu->popup(subjectView->viewport()->mapToGlobal(argPos));
    }
}

TabFileExplorer::~TabFileExplorer()
{
    delete ui;
}
