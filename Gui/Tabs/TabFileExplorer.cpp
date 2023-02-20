#include "DataModels/TabFileExplorer/TableModelFileExplorer.h"
#include "DataModels/TabFileExplorer/ListModelFileExplorer.h"
#include "ui_TabFileExplorer.h"

#include "TabFileExplorer.h"
#include "Utility/JsonDtoFormat.h"
#include "Backend/FileStorageSubSystem/FileStorageManager.h"

#include <QQueue>
#include <QThread>
#include <QMessageBox>
#include <QFileDialog>
#include <QtConcurrent>
#include <QStandardPaths>
#include <QProgressDialog>

TabFileExplorer::TabFileExplorer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabFileExplorer)
{
    ui->setupUi(this);

    dialogEditVersion = new DialogEditVersion(this);

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

void TabFileExplorer::thawFolderTree(const QString folderName,
                                     const QString &parentSymbolFolderPath,
                                     const QString &targetUserPath)
{
    QString nativePath = QDir::toNativeSeparators(targetUserPath);
    if(!nativePath.endsWith(QDir::separator()))
        nativePath.append(QDir::separator());

    QFutureWatcher<void> futureWatcher;
    QProgressDialog dialog(this);
    dialog.setLabelText(tr("Thawing folder <b>%1</b>...").arg(folderName));

    QObject::connect(&futureWatcher, &QFutureWatcher<void>::finished, &dialog, &QProgressDialog::reset);
    QObject::connect(&dialog, &QProgressDialog::canceled, &futureWatcher, &QFutureWatcher<void>::cancel);
    QObject::connect(&futureWatcher,  &QFutureWatcher<void>::progressRangeChanged, &dialog, &QProgressDialog::setRange);
    QObject::connect(&futureWatcher, &QFutureWatcher<void>::progressValueChanged,  &dialog, &QProgressDialog::setValue);

    QFuture<void> future = QtConcurrent::run([=] {

        QString currentUserPath = nativePath;
        auto fsm = FileStorageManager::instance();
        QJsonObject parentFolderJson = fsm->getFolderJsonBySymbolPath(parentSymbolFolderPath, true);

        QQueue<QJsonObject> symbolFolders;
        symbolFolders.enqueue(parentFolderJson);

        while(!symbolFolders.empty())
        {
            QJsonObject currentFolder = symbolFolders.dequeue();
            auto suffixPath = currentFolder[JsonKeys::Folder::SuffixPath].toString().chopped(1);
            currentUserPath.append(suffixPath).append(QDir::separator());

            QDir currentDir(currentUserPath);
            bool isCreated = currentDir.mkpath(currentUserPath);

            if(isCreated)
            {
                currentFolder[JsonKeys::Folder::UserFolderPath] = currentUserPath;
                currentFolder[JsonKeys::Folder::IsFrozen] = false;
                bool isUpdated = fsm->updateFolderEntity(currentFolder, true);
                if(isUpdated)
                    emit signalItemThawed(currentUserPath); // Notify about created folder

                QJsonArray childFiles = currentFolder[JsonKeys::Folder::ChildFiles].toArray();
                QJsonArray childFolders = currentFolder[JsonKeys::Folder::ChildFolders].toArray();

                for(const QJsonValue &currentChildFile : childFiles)
                {
                    QJsonObject fileJson = currentChildFile.toObject();
                    QJsonObject versionJson = fsm->getFileVersionJson(fileJson[JsonKeys::File::SymbolFilePath].toString(),
                                                                      fileJson[JsonKeys::File::MaxVersionNumber].toInteger());

                    QString internalFilePath = fsm->getBackupFolderPath();
                    internalFilePath.append(versionJson[JsonKeys::FileVersion::InternalFileName].toString());
                    QString userFilePath = currentUserPath + fileJson[JsonKeys::File::FileName].toString();

                    bool isCopied = QFile::copy(internalFilePath, userFilePath);
                    if(isCopied)
                        emit signalItemThawed(userFilePath); // Notify about copied file
                }

                for(const QJsonValue &currentChildFolder : childFolders)
                {
                    QString childFolderPath = currentChildFolder.toObject()[JsonKeys::Folder::SymbolFolderPath].toString();
                    symbolFolders.enqueue(fsm->getFolderJsonBySymbolPath(childFolderPath, true));
                }
            }
        }
    });

    futureWatcher.setFuture(future);
    dialog.exec();
    futureWatcher.waitForFinished();
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
        auto tableModel = (TableModelFileExplorer *) subjectView->model();
        bool isFrozen = tableModel->getIsFrozenFromModelIndex(index);

        if(isFrozen)
            ui->contextActionTableFileExplorer_Freeze->setText(tr("Thaw"));
        else
            ui->contextActionTableFileExplorer_Freeze->setText(tr("Freeze"));

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
        QString symbolPath = tableModel->getSymbolPathFromModelIndex(index);
        TableModelFileExplorer::TableItemType type = tableModel->getItemTypeFromModelIndex(index);

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
        QString symbolPath = model->getSymbolPathFromModelIndex(index);
        TableModelFileExplorer::TableItemType type = model->getItemTypeFromModelIndex(index);

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

void TabFileExplorer::on_contextActionListFileExplorer_EditVersion_triggered()
{
    QItemSelectionModel *tableViewSelectionModel = ui->tableViewFileExplorer->selectionModel();
    QModelIndexList listSymbolPath = tableViewSelectionModel->selectedRows(TableModelFileExplorer::ColumnIndexSymbolPath);
    auto fileSymbolPath = listSymbolPath.first().data().toString();

    QItemSelectionModel * listViewSelectionModel = ui->listView->selectionModel();
    QModelIndexList listVersionNumber = listViewSelectionModel->selectedRows(ListModelFileExplorer::ColumnIndexVersionNumber);
    auto versionNumber = listVersionNumber.first().data().toLongLong();

    dialogEditVersion->setModal(true);
    dialogEditVersion->show(fileSymbolPath, versionNumber);
}


void TabFileExplorer::on_contextActionTableFileExplorer_Delete_triggered()
{
    auto tableModel = (TableModelFileExplorer *) ui->tableViewFileExplorer->model();
    QModelIndex modelIndex = ui->tableViewFileExplorer->selectionModel()->selectedRows().first();

    TableModelFileExplorer::TableItemType type = tableModel->getItemTypeFromModelIndex(modelIndex);
    QString name = tableModel->getNameFromModelIndex(modelIndex);
    QString symbolPath = tableModel->getSymbolPathFromModelIndex(modelIndex);
    QString userPath = tableModel->getUserPathFromModelIndex(modelIndex);
    bool isFrozen = tableModel->getIsFrozenFromModelIndex(modelIndex);

    QString title = tr("Delete item ?");
    QString message;

    if(type == TableModelFileExplorer::TableItemType::Folder)
        message = tr("Deleting folder <b>%1</b> will remove everything inside it including the folder itself");
    else if(type == TableModelFileExplorer::TableItemType::File)
        message = tr("Deleting file <b>%1</b> will remove all versions of it");

    message = message.arg(name);

    QMessageBox::StandardButton result = QMessageBox::question(this, title, message);

    if(result == QMessageBox::StandardButton::Yes)
    {
        auto fsm = FileStorageManager::instance();

        if(type == TableModelFileExplorer::TableItemType::Folder)
        {
            fsm->deleteFolder(symbolPath);

            if(!isFrozen)
                emit signalActiveItemDeleted(userPath);
        }
        else if(type == TableModelFileExplorer::TableItemType::File)
        {
            fsm->deleteFile(symbolPath);

            if(!isFrozen)
                emit signalActiveItemDeleted(userPath);
        }

        slotRefreshFileExplorer();
    }
}


void TabFileExplorer::on_contextActionTableFileExplorer_Freeze_triggered()
{
    auto fsm = FileStorageManager::instance();
    auto tableModel = (TableModelFileExplorer *) ui->tableViewFileExplorer->model();
    QModelIndex modelIndex = ui->tableViewFileExplorer->selectionModel()->selectedRows().first();

    TableModelFileExplorer::TableItemType type = tableModel->getItemTypeFromModelIndex(modelIndex);
    QString name = tableModel->getNameFromModelIndex(modelIndex);
    QString symbolPath = tableModel->getSymbolPathFromModelIndex(modelIndex);
    QString userPath = tableModel->getUserPathFromModelIndex(modelIndex);
    bool isFrozen = tableModel->getIsFrozenFromModelIndex(modelIndex);

    if(type == TableModelFileExplorer::TableItemType::Folder)
    {
        QJsonObject folderJson = fsm->getFolderJsonBySymbolPath(symbolPath);

        if(isFrozen)
        {
            auto parentSymbolPath = folderJson[JsonKeys::Folder::ParentFolderPath].toString();
            QJsonObject parentFolderJson = fsm->getFolderJsonBySymbolPath(parentSymbolPath);

            if(parentFolderJson[JsonKeys::Folder::IsFrozen].toBool())
            {
                QString title = tr("Parent folder is frozen !");
                QString message = tr("Can't thaw child folder because parent is frozen. To thaw <b>%1</b>, thaw the parent first.");
                message = message.arg(name);
                QMessageBox::information(this, title, message);
                return;
            }

            QString title = tr("Select location for thawed folder");
            QString message = tr("Thawed folder <b>%1</b> and all content of it will be placed to location you'll select now.");
            message = message.arg(name);
            QMessageBox::information(this, title, message);

            QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::StandardLocation::DesktopLocation);
            QString selection = QFileDialog::getExistingDirectory(this,
                                                                  tr("Select location for folder to be thawed"),
                                                                  desktopPath,
                                                                  QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

            selection = QDir::toNativeSeparators(selection).append(QDir::separator());

            bool isExist = QDir(selection + name).exists();
            if(isExist)
            {
                QString title = tr("Folder exist at location !");
                QString message = tr("Can't overwrite folder <b>%1</b> in location you selected.");
                message = message.arg(name);
                QMessageBox::critical(this, title, message);
                return;
            }

            // TODO add checking empty space before extracting folders

            emit signalThawingStarted();
            thawFolderTree(name, symbolPath, selection);
            emit signalThawingFinished();
        }
        else
        {
            folderJson[JsonKeys::Folder::IsFrozen] = true;
            bool isUpdated = fsm->updateFolderEntity(folderJson, true);

            if(isUpdated)
                emit signalItemFrozen(userPath);
        }
    }
    else if(type == TableModelFileExplorer::TableItemType::File)
    {
        QJsonObject fileJson = fsm->getFileJsonBySymbolPath(symbolPath);

        if(isFrozen)
            fileJson[JsonKeys::File::IsFrozen] = false;
        else
        {
            fileJson[JsonKeys::File::IsFrozen] = true;
        }

        fsm->updateFileEntity(fileJson);

        if(isFrozen)
            emit signalItemThawed(userPath);
        else
            emit signalItemFrozen(userPath);
    }

    slotRefreshFileExplorer();
}

