#include "DataModels/TabFileExplorer/ListModelFileExplorer.h"
#include "FileStorageSubSystem/FileStorageManager.h"
#include "Tasks/TaskNaviagateFileSystem.h"
#include "ui_TabFileExplorer.h"

#include "TabFileExplorer.h"

#include <QThread>

TabFileExplorer::TabFileExplorer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabFileExplorer)
{
    ui->setupUi(this);
    buildContextMenuTableFileExplorer();
    buildContextMenuListFileExplorer();


    QList<TableModelFileExplorer::TableItem> sampleFileExplorerTableData;

    sampleFileExplorerTableData.insert(0, {"first_file", ".txt"});
    sampleFileExplorerTableData.insert(1, {"second_file", ".zip"});
    sampleFileExplorerTableData.insert(2, {"third_file", ".pdf"});
    sampleFileExplorerTableData.insert(3, {"fourth_file", ".mp4"});

    tableModelFileExplorer = new TableModelFileExplorer(sampleFileExplorerTableData, this);
    ui->tableViewFileExplorer->setModel(tableModelFileExplorer);

    QStringList sampleListData;
    sampleListData << "item 1" << "item 2" << "item 3" << "item 4" << "item 5";

    listModelFileExplorer = new ListModelFileExplorer(sampleListData, this);
    ui->listView->setModel(listModelFileExplorer);

    QObject::connect(ui->listView, &QListView::customContextMenuRequested, this, &TabFileExplorer::showContextMenuListView);
    QObject::connect(ui->tableViewFileExplorer, &QTableView::customContextMenuRequested, this, &TabFileExplorer::showContextMenuTableView);

    ui->lineEditWorkingDir->setText(FileStorageManager::rootFolderPath());
    fillFileExplorerWithRootFolderContents();

    createNavigationTask();
}

void TabFileExplorer::buildContextMenuTableFileExplorer()
{
    contextMenuTableFileExplorer = new QMenu(ui->tableViewFileExplorer);
    QMenu *ptrMenu = contextMenuTableFileExplorer;

    QAction *actionOpenFolderInNewTab = ui->contextActionTableFileExplorer_OpenFolderInNewTab;
    QAction *actionEdit = ui->contextActionTableFileExplorer_Edit;
    QAction *actionCut = ui->contextActionTableFileExplorer_Cut;
    QAction *actionFreeze = ui->contextActionTableFileExplorer_Freeze;
    QAction *actionDelete = ui->contextActionTableFileExplorer_Delete;

    ptrMenu->addAction(actionOpenFolderInNewTab);
    ptrMenu->addAction(actionEdit);
    ptrMenu->addAction(actionCut);
    ptrMenu->addAction(actionFreeze);
    ptrMenu->addAction(actionDelete);
}

void TabFileExplorer::buildContextMenuListFileExplorer()
{
    contextMenuListFileExplorer = new QMenu(ui->listView);
    QMenu *ptrMenu = contextMenuListFileExplorer;

    QAction *actionPreview = ui->contextActionListFileExplorer_Preview;
    QAction *actionEditVersion = ui->contextActionListFileExplorer_EditVersion;
    QAction *actionShowRelatedFiles = ui->contextActionListFileExplorer_ShowRelatedFiles;
    QAction *actionSchedule = ui->contextActionListFileExplorer_Schedule;
    QAction *actionScheduleAndOpenClipboard = ui->contextActionListFileExplorer_ScheduleAndOpenClipboard;
    QAction *actionSetAsCurrentVerion = ui->contextActionListFileExplorer_SetAsCurrentVersion;
    QAction *actionDelete = ui->contextActionListFileExplorer_Delete;

    ptrMenu->addAction(actionPreview);
    ptrMenu->addAction(actionEditVersion);
    ptrMenu->addAction(actionShowRelatedFiles);
    ptrMenu->addAction(actionSchedule);
    ptrMenu->addAction(actionScheduleAndOpenClipboard);
    ptrMenu->addAction(actionSetAsCurrentVerion);
    ptrMenu->addAction(actionDelete);
}

void TabFileExplorer::fillFileExplorerWithRootFolderContents()
{
    auto rootPath = FileStorageManager::rootFolderPath();
    auto result = FileStorageManager::instance()->getFolderMetaData(rootPath);
    onDirContentFetched(result);
}

void TabFileExplorer::createNavigationTask()
{
    navigationTaskControllerThread = new QThread(this);
    navigationTaskControllerThread->setObjectName(navigationTaskControllerThreadName());
    TaskNaviagateFileSystem *task = new TaskNaviagateFileSystem();

    QObject::connect(this, &TabFileExplorer::requestDirContent,
                     task, &TaskNaviagateFileSystem::onDirContentRequested);

    QObject::connect(task, &TaskNaviagateFileSystem::dirContentFetched,
                     this, &TabFileExplorer::onDirContentFetched);

    QObject::connect(navigationTaskControllerThread, &QThread::finished,
                     task, &QObject::deleteLater);

    task->moveToThread(navigationTaskControllerThread);
    navigationTaskControllerThread->start();
}

QString TabFileExplorer::navigationTaskControllerThreadName() const
{
    return "Navigation Task Controller Thread";
}


void TabFileExplorer::showContextMenuTableView(const QPoint &argPos)
{
    QAbstractItemView *subjectView = ui->tableViewFileExplorer;
    QModelIndex index = subjectView->indexAt(argPos);

    if(index.isValid()) // If user selected an item from table.
    {
        QMenu *ptrMenu = contextMenuTableFileExplorer;
        ptrMenu->popup(subjectView->viewport()->mapToGlobal(argPos));
    }
}

void TabFileExplorer::showContextMenuListView(const QPoint &argPos)
{
    QAbstractItemView *subjectView = ui->listView;
    QModelIndex index = subjectView->indexAt(argPos);

    if(index.isValid()) // If user selected an item from list.
    {
        QMenu *ptrMenu = contextMenuListFileExplorer;
        ptrMenu->popup(subjectView->viewport()->mapToGlobal(argPos));
    }
}

TabFileExplorer::~TabFileExplorer()
{
    navigationTaskControllerThread->quit();
    navigationTaskControllerThread->wait();

    delete ui;
}

void TabFileExplorer::on_contextActionListFileExplorer_ShowRelatedFiles_triggered()
{
    emit signalToRouter_ShowRelatedFiles();
}

void TabFileExplorer::on_contextActionTableFileExplorer_Edit_triggered()
{
    emit signalToRouter_ShowDialogTableItemEditor();
}

void TabFileExplorer::onDirContentFetched(FolderMetaData data)
{
    qDebug() << "TabFileExplorer::onDirContentFetched() in " << QThread::currentThread()->objectName();
}
