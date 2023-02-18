#include "DataModels/TabFileExplorer/TableModelFileExplorer.h"
#include "DataModels/TabFileExplorer/ListModelFileExplorer.h"
#include "ui_TabFileExplorer.h"

#include "TabFileExplorer.h"
#include "Utility/JsonDtoFormat.h"
#include "Backend/FileStorageSubSystem/FileStorageManager.h"

#include <QThread>
#include <QJsonArray>

TabFileExplorer::TabFileExplorer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabFileExplorer)
{
    ui->setupUi(this);

    buildContextMenuTableFileExplorer();
    buildContextMenuListFileExplorer();

    ui->buttonBack->setDisabled(true);
    ui->buttonForward->setDisabled(true);
    clearDescriptionDetails();

    this->ui->tableViewFileExplorer->horizontalHeader()->setMinimumSectionSize(110);
    this->ui->tableViewFileExplorer->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Interactive);

    ui->lineEditWorkingDir->setText(FileStorageManager::separator);
    displayFolderInTableViewFileExplorer(FileStorageManager::separator);
}

TabFileExplorer::~TabFileExplorer()
{
    delete ui;
}

void TabFileExplorer::buildContextMenuTableFileExplorer()
{
    contextMenuTableFileExplorer = new QMenu(ui->tableViewFileExplorer);
    QMenu *ptrMenu = contextMenuTableFileExplorer;

    QAction *actionOpenFolderInNewTab = ui->contextActionTableFileExplorer_OpenFolderInNewTab;
    QAction *actionCut = ui->contextActionTableFileExplorer_Cut;
    QAction *actionFreeze = ui->contextActionTableFileExplorer_Freeze;
    QAction *actionDelete = ui->contextActionTableFileExplorer_Delete;

    ptrMenu->addAction(actionOpenFolderInNewTab);
    ptrMenu->addAction(actionCut);
    ptrMenu->addAction(actionFreeze);
    ptrMenu->addAction(actionDelete);

    QObject::connect(ui->tableViewFileExplorer, &QTableView::customContextMenuRequested,
                     this, &TabFileExplorer::showContextMenuTableView);
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

    QObject::connect(ui->listView, &QListView::customContextMenuRequested,
                     this, &TabFileExplorer::showContextMenuListView);
}

void TabFileExplorer::createNavigationHistoryIndex(const QString &path)
{
    auto tokenList = path.split(FileStorageManager::separator);
    tokenList.removeLast();

    for(QString &token : tokenList)
        token.append(FileStorageManager::separator);

    QString aggregator;
    int index = 0;

    navigationHistoryIndices.clear();

    for(QString &token : tokenList)
    {
        aggregator.append(token);
        navigationHistoryIndices.append(aggregator);
    }
}

void TabFileExplorer::displayFolderInTableViewFileExplorer(const QString &symbolFolderPath)
{
    auto fsm = FileStorageManager::instance();
    QJsonObject folderJson = fsm->getFolderJsonBySymbolPath(symbolFolderPath, true);
    QTableView *tableView = ui->tableViewFileExplorer;

    if(tableView->model() != nullptr)
        delete tableView->model();

    auto tableModel = new TableModelFileExplorer(folderJson, this);
    tableView->setModel(tableModel);

    ui->lineEditWorkingDir->setText(folderJson[JsonKeys::Folder::SymbolFolderPath].toString());

    tableView->hideColumn(TableModelFileExplorer::ColumnIndexSymbolPath);
    tableView->hideColumn(TableModelFileExplorer::ColumnIndexItemType);
    tableView->viewport()->update();
    tableView->resizeColumnsToContents();
}

// https://stackoverflow.com/a/73912542
// https://stackoverflow.com/a/566085
// https://stackoverflow.com/questions/565704/how-to-correctly-convert-filesize-in-bytes-into-mega-or-gigabytes
QString TabFileExplorer::fileSizeToString(qulonglong fileSize) const
{
    QString result = "";

    qulonglong tb = Q_UINT64_C(1000 * 1000 * 1000 * 1000);
    qulonglong gb = Q_UINT64_C(1000 * 1000 * 1000);
    qulonglong mb = Q_UINT64_C(1000 * 1000);
    qulonglong kb = Q_UINT64_C(1000);

    double fileSizeDouble = (double) fileSize;

    if(fileSize >= tb)
        result = QString::number((fileSizeDouble/tb), 'g', 3) + " TB";

    else if(fileSize >= gb)
        result = QString::number((fileSizeDouble/gb), 'g', 3) + " GB";

    else if(fileSize >= mb)
        result = QString::number((fileSizeDouble/mb), 'g', 3) + " MB";

    else if(fileSize >= kb)
        result = QString::number((fileSizeDouble/kb), 'g', 4) + " KB";

    else
        result = QString::number(fileSize) + " bytes";

    return result;
}

QString TabFileExplorer::currentSymbolFolderPath() const
{
    return ui->lineEditWorkingDir->text();
}

void TabFileExplorer::slotRefreshFileExplorer()
{
    displayFolderInTableViewFileExplorer(ui->lineEditWorkingDir->text());
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

void TabFileExplorer::on_tableViewFileExplorer_clicked(const QModelIndex &index)
{
    if(index.isValid()) // If user clicked an item
    {
        auto tableModel = (TableModelFileExplorer *)index.model();
        QString symbolPath = tableModel->symbolPathFromModelIndex(index);
        TableModelFileExplorer::TableItemType type = tableModel->itemTypeFromModelIndex(index);

        if(type == TableModelFileExplorer::TableItemType::File)
        {
            ListModelFileExplorer *listModel = nullptr;
            if(ui->listView->model() != nullptr)
                delete ui->listView->model();

            auto fsm = FileStorageManager::instance();
            QJsonObject fileJson = fsm->getFileJsonBySymbolPath(symbolPath);

            listModel = new ListModelFileExplorer(fileJson, ui->listView);
            ui->listView->setModel(listModel);

            qlonglong latestRow = fileJson[JsonKeys::File::MaxVersionNumber].toInt() - 1;
            ui->listView->setCurrentIndex(listModel->index(latestRow, 0));
            on_listView_clicked(listModel->index(latestRow, 0));
        }
    }
}

void TabFileExplorer::on_tableViewFileExplorer_doubleClicked(const QModelIndex &index)
{
    if(index.isValid()) // If user double clicked an item
    {
        auto model = (TableModelFileExplorer *)index.model();
        QString symbolPath = model->symbolPathFromModelIndex(index);
        TableModelFileExplorer::TableItemType type = model->itemTypeFromModelIndex(index);

        if(type == TableModelFileExplorer::TableItemType::Folder)
        {
            createNavigationHistoryIndex(symbolPath);
            ui->buttonForward->setDisabled(true);

            // Enable back button whenever item is double clicked.
            ui->buttonBack->setEnabled(true);

            clearDescriptionDetails();
            displayFolderInTableViewFileExplorer(symbolPath);
        }
    }
}

void TabFileExplorer::on_listView_clicked(const QModelIndex &index)
{
    if(index.isValid())
    {
        auto model = (ListModelFileExplorer *) ui->listView->model();
        qlonglong versionNumber = model->data(index, Qt::ItemDataRole::DisplayRole).toLongLong();
        auto fsm = FileStorageManager::instance();
        QJsonObject fileVersionJson = fsm->getFileVersionJson(model->getFileSymbolPath(), versionNumber);

        QString size = fileSizeToString(fileVersionJson[JsonKeys::FileVersion::Size].toInteger());
        ui->labelDataSize->setText(size);
        ui->labelDataDate->setText(fileVersionJson[JsonKeys::FileVersion::Timestamp].toString());
        ui->textEditDescription->setText(fileVersionJson[JsonKeys::FileVersion::Description].toString());
    }
}

void TabFileExplorer::on_buttonBack_clicked()
{
    clearDescriptionDetails();

    if(ui->lineEditWorkingDir->text() != navigationHistoryIndices.first())
    {
        auto currentIndex = navigationHistoryIndices.indexOf(ui->lineEditWorkingDir->text());
        auto newIndex = currentIndex - 1;

        if(newIndex == 0)
            ui->buttonBack->setDisabled(true);

        ui->buttonForward->setEnabled(true);
        displayFolderInTableViewFileExplorer(navigationHistoryIndices.at(newIndex));
    }
}

void TabFileExplorer::on_buttonForward_clicked()
{
    clearDescriptionDetails();

    if(ui->lineEditWorkingDir->text() != navigationHistoryIndices.last())
    {
        auto currentIndex = navigationHistoryIndices.indexOf(ui->lineEditWorkingDir->text());
        auto newIndex = currentIndex + 1;

        if(newIndex == navigationHistoryIndices.size() - 1)
            ui->buttonForward->setDisabled(true);

        ui->buttonBack->setEnabled(true);
        displayFolderInTableViewFileExplorer(navigationHistoryIndices.at(newIndex));
    }
}

void TabFileExplorer::clearDescriptionDetails()
{
    ui->labelDataSize->setText("-");
    ui->labelDataDate->setText("-");
    ui->textEditDescription->setText("");

    QAbstractItemModel *listModel = ui->listView->model();

    if(listModel != nullptr)
        delete listModel;
}
