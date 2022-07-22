#include "TabFileMonitor.h"
#include "ui_TabFileMonitor.h"
#include "Backend/FileStorageSubSystem/FileStorageManager.h"

#include <QtConcurrent>
#include <QFileInfo>
#include <QDir>

TabFileMonitor::TabFileMonitor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabFileMonitor)
{
    ui->setupUi(this);

    this->comboBoxItemDelegateNote = new ComboBoxItemDelegateNote(this->ui->tableViewFileMonitor);
    this->comboBoxItemDelegateFileAction = new ComboBoxItemDelegateFileAction(this->ui->tableViewFileMonitor);
}

TabFileMonitor::~TabFileMonitor()
{
    delete ui;
}

void TabFileMonitor::slotOnPredictionTargetNotFound(const QString &pathToFile)
{
    // TODO this will probably require another item
    QFileInfo fileInfo(pathToFile);
    auto fileDir = QDir::toNativeSeparators(fileInfo.absolutePath()) + QDir::separator();
    TableModelFileMonitor::TableItem item {
        fileInfo.fileName(),
        fileDir,
        "",
        TableModelFileMonitor::TableItemType::File,
        TableModelFileMonitor::TableItemStatus::Missing,
        QDateTime::currentDateTime()
    };

    addRowToTableViewFileMonitor(item);
}

void TabFileMonitor::slotOnUnPredictedFolderDetected(const QString &pathToFolder)
{
    auto item = TableModelFileMonitor::tableItemNewAddedFolderFrom(pathToFolder);
    addRowToTableViewFileMonitor(item);
}

void TabFileMonitor::slotOnNewFolderAdded(const QString &pathToFolder)
{
    auto *watcher = new QFutureWatcher<TableModelFileMonitor::TableItem>(this);
    resultSet.insert(watcher);
    QObject::connect(watcher, &QFutureWatcher<TableModelFileMonitor::TableItem>::finished,
                     this, &TabFileMonitor::slotRefreshTableViewFileMonitor);

    auto future = QtConcurrent::run([=]{
                      auto fsm = FileStorageManager::instance();
                      bool isFolderExistInDb = fsm->isFolderExistByUserFolderPath(pathToFolder);
                      return isFolderExistInDb;

                  }).then(QtFuture::Launch::Inherit, [=](QFuture<bool> previous){
                          TableModelFileMonitor::TableItem item;

                          if(previous.result() == true)
                              item = TableModelFileMonitor::tableItemModifiedFolderFrom(pathToFolder);
                          else
                              item = TableModelFileMonitor::tableItemNewAddedFolderFrom(pathToFolder);

                          return item;
                      });

    watcher->setFuture(future);

//    auto item = TableModelFileMonitor::tableItemNewAddedFolderFrom(pathToFolder);
//    addRowToTableViewFileMonitor(item);
}

void TabFileMonitor::slotOnFolderDeleted(const QString &pathToFolder)
{
    auto *watcher = new QFutureWatcher<TableModelFileMonitor::TableItem>(this);
    resultSet.insert(watcher);
    QObject::connect(watcher, &QFutureWatcher<TableModelFileMonitor::TableItem>::finished,
                     this, &TabFileMonitor::slotRefreshTableViewFileMonitor);

    auto future = QtConcurrent::run([=]{
                      auto fsm = FileStorageManager::instance();
                      bool isFolderExistInDb = fsm->isFolderExistByUserFolderPath(pathToFolder);
                      return isFolderExistInDb;

                  }).then(QtFuture::Launch::Inherit, [=](QFuture<bool> previous){
                          TableModelFileMonitor::TableItem item;

                          if(previous.result() == true)
                              item = TableModelFileMonitor::tableItemDeletedFolderFrom(pathToFolder);

                          return item;
                      });

    watcher->setFuture(future);

//    auto item = TableModelFileMonitor::tableItemDeletedFolderFrom(pathToFolder);
//    addRowToTableViewFileMonitor(item);
}

void TabFileMonitor::slotOnFolderMoved(const QString &pathToFolder, const QString &oldFolderName)
{
    QDir parentDir = QFileInfo(pathToFolder).absoluteDir();
    parentDir.cdUp();

    auto pathToOldFolder = QDir::toNativeSeparators(parentDir.absolutePath()) + QDir::separator();
    pathToOldFolder += oldFolderName + QDir::separator();

    auto *watcher = new QFutureWatcher<TableModelFileMonitor::TableItem>(this);
    resultSet.insert(watcher);
    QObject::connect(watcher, &QFutureWatcher<TableModelFileMonitor::TableItem>::finished,
                     this, &TabFileMonitor::slotRefreshTableViewFileMonitor);

    auto future = QtConcurrent::run([=]{
                      auto fsm = FileStorageManager::instance();
                      bool isFolderExistInDb = fsm->isFolderExistByUserFolderPath(pathToOldFolder);
                      return isFolderExistInDb;

                  }).then(QtFuture::Launch::Inherit, [=](QFuture<bool> previous){
                          TableModelFileMonitor::TableItem item;

                          if(previous.result() == true)
                              item = TableModelFileMonitor::tableItemMovedFolderFrom(pathToFolder, oldFolderName);

                          return item;
                      });

    watcher->setFuture(future);

//    auto item = TableModelFileMonitor::tableItemMovedFolderFrom(pathToFolder, oldFolderName);
//    addRowToTableViewFileMonitor(item);
}

void TabFileMonitor::slotOnUnPredictedFileDetected(const QString &pathToFile)
{
    auto item = TableModelFileMonitor::tableItemNewAddedFileFrom(pathToFile);
    addRowToTableViewFileMonitor(item);
}

void TabFileMonitor::slotOnNewFileAdded(const QString &pathToFile)
{
    auto *watcher = new QFutureWatcher<TableModelFileMonitor::TableItem>(this);
    resultSet.insert(watcher);
    QObject::connect(watcher, &QFutureWatcher<TableModelFileMonitor::TableItem>::finished,
                     this, &TabFileMonitor::slotRefreshTableViewFileMonitor);

    auto future = QtConcurrent::run([=]{

        TableModelFileMonitor::TableItem item;
        auto fsm = FileStorageManager::instance();

        bool isFileExistInDb = fsm->isFileExistByUserFilePath(pathToFile);

        if(isFileExistInDb)
            item = TableModelFileMonitor::tableItemModifiedFileFrom(pathToFile);
        else
            item = TableModelFileMonitor::tableItemNewAddedFileFrom(pathToFile);

        return item;
    });

    watcher->setFuture(future);

//    auto item = TableModelFileMonitor::tableItemNewAddedFileFrom(pathToFile);
//    addRowToTableViewFileMonitor(item);
}

void TabFileMonitor::slotOnFileDeleted(const QString &pathToFile)
{
    auto *watcher = new QFutureWatcher<TableModelFileMonitor::TableItem>(this);
    resultSet.insert(watcher);
    QObject::connect(watcher, &QFutureWatcher<TableModelFileMonitor::TableItem>::finished,
                     this, &TabFileMonitor::slotRefreshTableViewFileMonitor);

    auto future = QtConcurrent::run([=]{

        TableModelFileMonitor::TableItem item;
        auto fsm = FileStorageManager::instance();

        bool isFileExistInDb = fsm->isFileExistByUserFilePath(pathToFile);

        if(isFileExistInDb)
            item = TableModelFileMonitor::tableItemDeletedFileFrom(pathToFile);

        return item;
    });

    watcher->setFuture(future);

//    auto item = TableModelFileMonitor::tableItemDeletedFileFrom(pathToFile);
//    addRowToTableViewFileMonitor(item);
}

void TabFileMonitor::slotOnFileMoved(const QString &pathToFile, const QString &oldFileName)
{
    QString pathToOldFile = QFileInfo(pathToFile).absolutePath();
    pathToOldFile = QDir::toNativeSeparators(pathToOldFile) + QDir::separator();
    pathToOldFile += oldFileName;

    auto *watcher = new QFutureWatcher<TableModelFileMonitor::TableItem>(this);
    resultSet.insert(watcher);
    QObject::connect(watcher, &QFutureWatcher<TableModelFileMonitor::TableItem>::finished,
                     this, &TabFileMonitor::slotRefreshTableViewFileMonitor);

    auto future = QtConcurrent::run([=]{
                      auto fsm = FileStorageManager::instance();
                      bool isFileExistInDb = fsm->isFileExistByUserFilePath(pathToOldFile);
                      return isFileExistInDb;

                  }).then(QtFuture::Launch::Inherit, [=](QFuture<bool> previous){
                          TableModelFileMonitor::TableItem item;

                          if(previous.result() == true)
                              item = TableModelFileMonitor::tableItemMovedFileFrom(pathToFile, oldFileName);

                          return item;
                      });

    watcher->setFuture(future);

//    auto item = TableModelFileMonitor::tableItemMovedFileFrom(pathToFile, oldFileName);
//    addRowToTableViewFileMonitor(item);
}

void TabFileMonitor::slotOnFileModified(const QString &pathToFile)
{
    auto *watcher = new QFutureWatcher<TableModelFileMonitor::TableItem>(this);
    resultSet.insert(watcher);
    QObject::connect(watcher, &QFutureWatcher<TableModelFileMonitor::TableItem>::finished,
                     this, &TabFileMonitor::slotRefreshTableViewFileMonitor);

    auto future = QtConcurrent::run([=]{
        auto fsm = FileStorageManager::instance();
        bool isFileExistInDb = fsm->isFileExistByUserFilePath(pathToFile);
        return isFileExistInDb;

    }).then(QtFuture::Launch::Inherit, [=](QFuture<bool> previous){
        TableModelFileMonitor::TableItem item;

        if(previous.result() == true)
            item = TableModelFileMonitor::tableItemModifiedFileFrom(pathToFile);

        return item;
    });

    watcher->setFuture(future);

//    auto item = TableModelFileMonitor::tableItemUpdatedFileFrom(pathToFile);
//    addRowToTableViewFileMonitor(item);
}

void TabFileMonitor::slotOnFileMovedAndModified(const QString &pathToFile, const QString &oldFileName)
{
    QString pathToOldFile = QFileInfo(pathToFile).absolutePath();
    pathToOldFile = QDir::toNativeSeparators(pathToOldFile) + QDir::separator();
    pathToOldFile += oldFileName;

    auto *watcher = new QFutureWatcher<TableModelFileMonitor::TableItem>(this);
    resultSet.insert(watcher);
    QObject::connect(watcher, &QFutureWatcher<TableModelFileMonitor::TableItem>::finished,
                     this, &TabFileMonitor::slotRefreshTableViewFileMonitor);

    auto future = QtConcurrent::run([=]{
                      auto fsm = FileStorageManager::instance();
                      bool isFileExistInDb = fsm->isFileExistByUserFilePath(pathToOldFile);
                      return isFileExistInDb;

                  }).then(QtFuture::Launch::Inherit, [=](QFuture<bool> previous){
                          TableModelFileMonitor::TableItem item;

                          if(previous.result() == true)
                              item = TableModelFileMonitor::tableItemMovedAndModifiedFileFrom(pathToFile, oldFileName);

                          return item;
                      });

    watcher->setFuture(future);

//    auto item = TableModelFileMonitor::tableItemMovedAndUpdatedFileFrom(pathToFile, oldFileName);
//    addRowToTableViewFileMonitor(item);
}

void TabFileMonitor::slotRefreshTableViewFileMonitor()
{
    for(QFutureWatcher<TableModelFileMonitor::TableItem> *watcher : qAsConst(resultSet))
    {
        if(watcher->isFinished())
        {
            addRowToTableViewFileMonitor(watcher->result());
            watcher->deleteLater();
            resultSet.remove(watcher);
        }
    }
}

void TabFileMonitor::addRowToTableViewFileMonitor(const TableModelFileMonitor::TableItem &item)
{
    if(item.status != TableModelFileMonitor::TableItemStatus::InvalidStatus)
    {
        TableModelFileMonitor *tableModel = (TableModelFileMonitor *) ui->tableViewFileMonitor->model();

        if(tableModel == nullptr)
        {
            tableModel = new TableModelFileMonitor({item}, ui->tableViewFileMonitor);
            ui->tableViewFileMonitor->setModel(tableModel);
        }
        else
        {
            QList<TableModelFileMonitor::TableItem> itemList = tableModel->getItemList();
            bool isExist = itemList.contains(item);

            if(isExist)
            {
                auto index = itemList.indexOf(item);
                itemList.replace(index, item);
            }
            else
                itemList.append(item);

            delete tableModel;
            tableModel = new TableModelFileMonitor(itemList, ui->tableViewFileMonitor);
            ui->tableViewFileMonitor->setModel(tableModel);
        }

        ui->tableViewFileMonitor->horizontalHeader()->setMinimumSectionSize(110);
        ui->tableViewFileMonitor->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
        ui->tableViewFileMonitor->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeMode::Interactive);
        ui->tableViewFileMonitor->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeMode::Interactive);
        ui->tableViewFileMonitor->resizeColumnsToContents();
    }
}
