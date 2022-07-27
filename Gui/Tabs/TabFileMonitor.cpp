#include "TabFileMonitor.h"
#include "ui_TabFileMonitor.h"
#include "Tasks/LambdaFactoryTabFileMonitor.h"
#include "Backend/FileStorageSubSystem/FileStorageManager.h"
#include "DataModels/TabFileMonitor/V2TableModelFileMonitor.h"

#include <QtConcurrent>
#include <QSqlQuery>
#include <QFileInfo>
#include <QDir>

TabFileMonitor::TabFileMonitor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabFileMonitor)
{
    ui->setupUi(this);
    createDb();

    this->comboBoxItemDelegateNote = new ComboBoxItemDelegateNote(this->ui->tableViewFileMonitor);
    this->comboBoxItemDelegateFileAction = new ComboBoxItemDelegateFileAction(this->ui->tableViewFileMonitor);
}

TabFileMonitor::~TabFileMonitor()
{
    delete ui;
}

QString TabFileMonitor::dbConnectionName()
{
    return "TabFileMonitorSqlFile";
}

QString TabFileMonitor::dbFileName()
{
    return "file:TabFileMonitorSqlFile?mode=memory&cache=shared";
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
    auto *watcher = new QFutureWatcher<void>(this);
    newResultSet.insert(watcher);
    QObject::connect(watcher, &QFutureWatcher<void>::finished,
                     this, &TabFileMonitor::slotRefreshTableViewFileMonitor);

    QFuture<void> future = QtConcurrent::run([=]{

        std::function<bool (QString)> lambdaIsExistInDb = LambdaFactoryTabFileMonitor::lambdaIsFileExistInDb();
        bool isExistInDb = lambdaIsExistInDb(pathToFile);

        std::function<bool (QString, QString)> lambdaIsExistInModelDb;
        lambdaIsExistInModelDb = LambdaFactoryTabFileMonitor::lambdaIsFileRowExistInModelDb();
        bool isExistInModelDb = lambdaIsExistInModelDb(dbConnectionName(), pathToFile);

        if(isExistInDb)
        {
            if(isExistInModelDb)
            {
                std::function<void (QString, QString, V2TableModelFileMonitor::TableItemStatus)> lambdaUpdate;
                lambdaUpdate = LambdaFactoryTabFileMonitor::lambdaUpdateStatusOfFileRowInModelDb();
                lambdaUpdate(dbConnectionName(), pathToFile, V2TableModelFileMonitor::TableItemStatus::Modified);
            }
            else
            {
                std::function<void (QString, QString, V2TableModelFileMonitor::TableItemStatus)> lambdaInsert;
                lambdaInsert = LambdaFactoryTabFileMonitor::lambdaInsertFileRowIntoModelDb();
                lambdaInsert(dbConnectionName(), pathToFile, V2TableModelFileMonitor::TableItemStatus::Modified);
            }
        }
        else
        {
            if(isExistInModelDb)
            {
                std::function<void (QString, QString, V2TableModelFileMonitor::TableItemStatus)> lambdaUpdate;
                lambdaUpdate = LambdaFactoryTabFileMonitor::lambdaUpdateStatusOfFileRowInModelDb();
                lambdaUpdate(dbConnectionName(), pathToFile, V2TableModelFileMonitor::TableItemStatus::NewAdded);
            }
            else
            {
                std::function<void (QString, QString, V2TableModelFileMonitor::TableItemStatus)> lambdaInsert;
                lambdaInsert = LambdaFactoryTabFileMonitor::lambdaInsertFileRowIntoModelDb();
                lambdaInsert(dbConnectionName(), pathToFile, V2TableModelFileMonitor::TableItemStatus::NewAdded);
            }
        }
    });

    watcher->setFuture(future);
}

void TabFileMonitor::slotOnFileDeleted(const QString &pathToFile)
{
    auto *watcher = new QFutureWatcher<void>(this);
    newResultSet.insert(watcher);
    QObject::connect(watcher, &QFutureWatcher<void>::finished,
                     this, &TabFileMonitor::slotRefreshTableViewFileMonitor);

    QFuture<void> future = QtConcurrent::run([=]{

        std::function<bool (QString)> lambdaIsExistInDb = LambdaFactoryTabFileMonitor::lambdaIsFileExistInDb();
        bool isExistInDb = lambdaIsExistInDb(pathToFile);

        std::function<bool (QString, QString)> lambdaIsExistInModelDb;
        lambdaIsExistInModelDb = LambdaFactoryTabFileMonitor::lambdaIsFileRowExistInModelDb();
        bool isExistInModelDb = lambdaIsExistInModelDb(dbConnectionName(), pathToFile);

        if(isExistInDb)
        {
            if(isExistInModelDb)
            {
                std::function<void (QString, QString, V2TableModelFileMonitor::TableItemStatus)> lambdaUpdate;
                lambdaUpdate = LambdaFactoryTabFileMonitor::lambdaUpdateStatusOfFileRowInModelDb();
                lambdaUpdate(dbConnectionName(), pathToFile, V2TableModelFileMonitor::TableItemStatus::Deleted);
            }
            else
            {
                std::function<void (QString, QString, V2TableModelFileMonitor::TableItemStatus)> lambdaInsert;
                lambdaInsert = LambdaFactoryTabFileMonitor::lambdaInsertFileRowIntoModelDb();
                lambdaInsert(dbConnectionName(), pathToFile, V2TableModelFileMonitor::TableItemStatus::Deleted);
            }
        }
        else
        {
            if(isExistInModelDb)
            {
                std::function<void (QString, QString, V2TableModelFileMonitor::TableItemStatus)> lambdaUpdate;
                lambdaUpdate = LambdaFactoryTabFileMonitor::lambdaUpdateStatusOfFileRowInModelDb();
                lambdaUpdate(dbConnectionName(), pathToFile, V2TableModelFileMonitor::TableItemStatus::Deleted);
            }
        }
    });

    watcher->setFuture(future);
}

void TabFileMonitor::slotOnFileMoved(const QString &pathToFile, const QString &oldFileName)
{
    QString pathToOldFile = QDir::toNativeSeparators(QFileInfo(pathToFile).absolutePath()) + QDir::separator();
    pathToOldFile += oldFileName;

    auto *watcher = new QFutureWatcher<void>(this);
    newResultSet.insert(watcher);
    QObject::connect(watcher, &QFutureWatcher<void>::finished,
                     this, &TabFileMonitor::slotRefreshTableViewFileMonitor);

    QFuture<void> future = QtConcurrent::run([=]{

        std::function<bool (QString, QString)> lambdaIsOldFileExistInModelDb;
        lambdaIsOldFileExistInModelDb = LambdaFactoryTabFileMonitor::lambdaIsFileRowExistInModelDb();
        bool isOldFileExistInModelDb = lambdaIsOldFileExistInModelDb(dbConnectionName(), pathToOldFile);

        if(isOldFileExistInModelDb)
        {
            std::function<void (QString, QString, V2TableModelFileMonitor::TableItemStatus)> lambdaUpdate;
            lambdaUpdate = LambdaFactoryTabFileMonitor::lambdaUpdateStatusOfFileRowInModelDb();
            lambdaUpdate(dbConnectionName(), pathToOldFile, V2TableModelFileMonitor::TableItemStatus::Deleted);
        }
        else
        {
            std::function<void (QString, QString, V2TableModelFileMonitor::TableItemStatus)> lambdaInsert;
            lambdaInsert = LambdaFactoryTabFileMonitor::lambdaInsertFileRowIntoModelDb();
            lambdaInsert(dbConnectionName(), pathToOldFile, V2TableModelFileMonitor::TableItemStatus::Deleted);
        }

        std::function<void (QString, QString, V2TableModelFileMonitor::TableItemStatus)> lambdaInsert;
        lambdaInsert = LambdaFactoryTabFileMonitor::lambdaInsertFileRowIntoModelDb();
        lambdaInsert(dbConnectionName(), pathToFile, V2TableModelFileMonitor::TableItemStatus::NewAdded);

        std::function<bool (QString)> lambdaIsOldFileExistInDb;
        lambdaIsOldFileExistInDb = LambdaFactoryTabFileMonitor::lambdaIsFileExistInDb();
        bool isNewFileExistInDb = lambdaIsOldFileExistInDb(pathToFile);

        if(isNewFileExistInDb)
        {
            std::function<void (QString, QString)> lambdaDelete;
            lambdaDelete = LambdaFactoryTabFileMonitor::lambdaDeleteFileRowFromModelDb();
            lambdaDelete(dbConnectionName(), pathToFile);
        }
    });

    watcher->setFuture(future);
}

void TabFileMonitor::slotOnFileModified(const QString &pathToFile)
{
    auto *watcher = new QFutureWatcher<void>(this);
    newResultSet.insert(watcher);
    QObject::connect(watcher, &QFutureWatcher<void>::finished,
                     this, &TabFileMonitor::slotRefreshTableViewFileMonitor);

    QFuture<void> future = QtConcurrent::run([=]{

        std::function<bool (QString)> lambdaIsExistInDb = LambdaFactoryTabFileMonitor::lambdaIsFileExistInDb();
        bool isExistInDb = lambdaIsExistInDb(pathToFile);
        if(isExistInDb)
        {
            std::function<bool (QString, QString)> lambdaIsExistInModelDb;
            lambdaIsExistInModelDb = LambdaFactoryTabFileMonitor::lambdaIsFileRowExistInModelDb();
            bool isExistInModelDb = lambdaIsExistInModelDb(dbConnectionName(), pathToFile);

            if(isExistInModelDb)
            {
                std::function<void (QString, QString, V2TableModelFileMonitor::TableItemStatus)> lambdaUpdate;
                lambdaUpdate = LambdaFactoryTabFileMonitor::lambdaUpdateStatusOfFileRowInModelDb();
                lambdaUpdate(dbConnectionName(), pathToFile, V2TableModelFileMonitor::TableItemStatus::Modified);
            }
            else
            {
                std::function<void (QString, QString, V2TableModelFileMonitor::TableItemStatus)> lambdaInsert;
                lambdaInsert = LambdaFactoryTabFileMonitor::lambdaInsertFileRowIntoModelDb();
                lambdaInsert(dbConnectionName(), pathToFile, V2TableModelFileMonitor::TableItemStatus::Modified);
            }
        }
    });

    watcher->setFuture(future);
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
//    for(QFutureWatcher<TableModelFileMonitor::TableItem> *watcher : qAsConst(resultSet))
//    {
//        if(watcher->isFinished())
//        {
//            addRowToTableViewFileMonitor(watcher->result());
//            watcher->deleteLater();
//            resultSet.remove(watcher);
//        }
//    }
    for(QFutureWatcher<void> *watcher : qAsConst(newResultSet))
    {
        if(watcher->isFinished())
        {
            TableModelFileMonitor::TableItem dummy;
            addRowToTableViewFileMonitor(dummy);
            watcher->deleteLater();
            newResultSet.remove(watcher);
        }
    }
}

void TabFileMonitor::addRowToTableViewFileMonitor(const TableModelFileMonitor::TableItem &item)
{
    V2TableModelFileMonitor *tableModel = (V2TableModelFileMonitor *) ui->tableViewFileMonitor->model();

    if(tableModel == nullptr)
    {
        tableModel = new V2TableModelFileMonitor(ui->tableViewFileMonitor);
        ui->tableViewFileMonitor->setModel(tableModel);
    }

    tableModel->setQuery("SELECT * FROM TableItem;", db);

    ui->tableViewFileMonitor->horizontalHeader()->setMinimumSectionSize(110);
    ui->tableViewFileMonitor->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);

    ui->tableViewFileMonitor->horizontalHeader()->setSectionResizeMode(V2TableModelFileMonitor::ColumnIndex::Name,
                                                                       QHeaderView::ResizeMode::Interactive);

    ui->tableViewFileMonitor->horizontalHeader()->setSectionResizeMode(V2TableModelFileMonitor::ColumnIndex::ParentDir,
                                                                       QHeaderView::ResizeMode::Interactive);
    ui->tableViewFileMonitor->resizeColumnsToContents();

//    if(item.status != TableModelFileMonitor::TableItemStatus::InvalidStatus)
//    {
//        TableModelFileMonitor *tableModel = (TableModelFileMonitor *) ui->tableViewFileMonitor->model();

//        if(tableModel == nullptr)
//        {
//            tableModel = new TableModelFileMonitor({item}, ui->tableViewFileMonitor);
//            ui->tableViewFileMonitor->setModel(tableModel);
//        }
//        else
//        {
//            QList<TableModelFileMonitor::TableItem> itemList = tableModel->getItemList();
//            bool isExist = itemList.contains(item);

//            if(isExist)
//            {
//                auto index = itemList.indexOf(item);
//                itemList.replace(index, item);
//            }
//            else
//                itemList.append(item);

//            delete tableModel;
//            tableModel = new TableModelFileMonitor(itemList, ui->tableViewFileMonitor);
//            ui->tableViewFileMonitor->setModel(tableModel);
//        }

//        ui->tableViewFileMonitor->horizontalHeader()->setMinimumSectionSize(110);
//        ui->tableViewFileMonitor->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
//        ui->tableViewFileMonitor->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeMode::Interactive);
//        ui->tableViewFileMonitor->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeMode::Interactive);
//        ui->tableViewFileMonitor->resizeColumnsToContents();
//    }
}

void TabFileMonitor::createDb()
{
    db = QSqlDatabase::addDatabase("QSQLITE", dbConnectionName());
    db.setConnectOptions("QSQLITE_OPEN_URI;QSQLITE_ENABLE_SHARED_CACHE");

//    db.setDatabaseName(dbFileName());

    auto path = QStandardPaths::writableLocation(QStandardPaths::StandardLocation::DesktopLocation);
    path = QDir::toNativeSeparators(path) + QDir::separator();
    path += "tableMonitor.db";
    db.setDatabaseName(path);

    db.open();

    QString queryString = "CREATE TABLE \"TableItem\" (";
    queryString += " \"name\" TEXT NOT NULL,";
    queryString += " \"parent_dir\" TEXT NOT NULL,";
    queryString += " \"path\" TEXT NOT NULL UNIQUE GENERATED ALWAYS AS (\"parent_dir\" || \"name\") VIRTUAL,";
    queryString += " \"old_name\" TEXT,";
    queryString += " \"type\" INTEGER NOT NULL CHECK(\"type\" >= 0 AND \"type\" <= 2),";
    queryString += " \"status\"	INTEGER NOT NULL CHECK(\"status\" >= 0 AND \"status\" <= 6),";
    queryString += " \"timestamp\" TEXT NOT NULL,";
    queryString += " \"action\"	INTEGER,";
    queryString += " \"note_number\" INTEGER,";
    queryString += " PRIMARY KEY(\"parent_dir\", \"name\")";
    queryString += ");";

    QSqlQuery query(db);
    query.prepare(queryString);
    query.exec();
}
