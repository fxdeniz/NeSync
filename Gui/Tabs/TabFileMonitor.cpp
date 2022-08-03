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

void TabFileMonitor::slotOnPredictionTargetNotFound(const QString &pathToFileOrFolder)
{
    auto *watcher = new QFutureWatcher<void>(this);
    newResultSet.insert(watcher);
    QObject::connect(watcher, &QFutureWatcher<void>::finished,
                     this, &TabFileMonitor::slotRefreshTableViewFileMonitor);

    QFuture<void> future = QtConcurrent::run([=]{
        std::function<void (QString, QString, V2TableModelFileMonitor::TableItemStatus)> lambdaInsert;
        lambdaInsert = LambdaFactoryTabFileMonitor::lambdaInsertRowIntoModelDb();
        lambdaInsert(dbConnectionName(), pathToFileOrFolder, V2TableModelFileMonitor::TableItemStatus::Missing);
    });

    watcher->setFuture(future);
}

void TabFileMonitor::slotOnUnPredictedFolderDetected(const QString &pathToFolder)
{
    auto item = TableModelFileMonitor::tableItemNewAddedFolderFrom(pathToFolder);
    addRowToTableViewFileMonitor(item);
}

void TabFileMonitor::slotOnNewFolderAdded(const QString &pathToFolder)
{
    auto *watcher = new QFutureWatcher<void>(this);
    newResultSet.insert(watcher);
    QObject::connect(watcher, &QFutureWatcher<void>::finished,
                     this, &TabFileMonitor::slotRefreshTableViewFileMonitor);

    QFuture<void> future = QtConcurrent::run([=]{

        std::function<bool (QString)> lambdaIsExistInDb;
        lambdaIsExistInDb = LambdaFactoryTabFileMonitor::lambdaIsFolderExistInDb();

        std::function<bool (QString, QString)> lambdaIsExistInModelDb;
        lambdaIsExistInModelDb = LambdaFactoryTabFileMonitor::lambdaIsRowExistInModelDb();

        std::function<void (QString, QString, V2TableModelFileMonitor::TableItemStatus)> lambdaUpdate;
        lambdaUpdate = LambdaFactoryTabFileMonitor::lambdaUpdateStatusOfRowInModelDb();

        std::function<void (QString, QString, V2TableModelFileMonitor::TableItemStatus)> lambdaInsert;
        lambdaInsert = LambdaFactoryTabFileMonitor::lambdaInsertRowIntoModelDb();

        bool isExistInDb = lambdaIsExistInDb(pathToFolder);
        bool isExistInModelDb = lambdaIsExistInModelDb(dbConnectionName(), pathToFolder);

        if(isExistInDb)
        {
            if(isExistInModelDb)
                lambdaUpdate(dbConnectionName(), pathToFolder, V2TableModelFileMonitor::TableItemStatus::Modified);
            else
                lambdaInsert(dbConnectionName(), pathToFolder, V2TableModelFileMonitor::TableItemStatus::Modified);
        }
        else
        {
            if(isExistInModelDb)
                lambdaUpdate(dbConnectionName(), pathToFolder, V2TableModelFileMonitor::TableItemStatus::NewAdded);
            else
                lambdaInsert(dbConnectionName(), pathToFolder, V2TableModelFileMonitor::TableItemStatus::NewAdded);
        }
    });

    watcher->setFuture(future);
}

void TabFileMonitor::slotOnFolderDeleted(const QString &pathToFolder)
{
    auto *watcher = new QFutureWatcher<void>(this);
    newResultSet.insert(watcher);
    QObject::connect(watcher, &QFutureWatcher<void>::finished,
                     this, &TabFileMonitor::slotRefreshTableViewFileMonitor);

    QFuture<void> future = QtConcurrent::run([=]{

        std::function<bool (QString)> lambdaIsExistInDb = LambdaFactoryTabFileMonitor::lambdaIsFolderExistInDb();
        bool isExistInDb = lambdaIsExistInDb(pathToFolder);

        std::function<bool (QString, QString)> lambdaIsExistInModelDb;
        lambdaIsExistInModelDb = LambdaFactoryTabFileMonitor::lambdaIsRowExistInModelDb();
        bool isExistInModelDb = lambdaIsExistInModelDb(dbConnectionName(), pathToFolder);

        if(isExistInDb)
        {
            if(isExistInModelDb)
            {
                std::function<void (QString, QString, V2TableModelFileMonitor::TableItemStatus)> lambdaUpdate;
                lambdaUpdate = LambdaFactoryTabFileMonitor::lambdaUpdateStatusOfRowInModelDb();
                lambdaUpdate(dbConnectionName(), pathToFolder, V2TableModelFileMonitor::TableItemStatus::Deleted);
            }
            else
            {
                std::function<void (QString, QString, V2TableModelFileMonitor::TableItemStatus)> lambdaInsert;
                lambdaInsert = LambdaFactoryTabFileMonitor::lambdaInsertRowIntoModelDb();
                lambdaInsert(dbConnectionName(), pathToFolder, V2TableModelFileMonitor::TableItemStatus::Deleted);
            }
        }
        else
        {
            if(isExistInModelDb)
            {
                std::function<void (QString, QString, V2TableModelFileMonitor::TableItemStatus)> lambdaUpdate;
                lambdaUpdate = LambdaFactoryTabFileMonitor::lambdaUpdateStatusOfRowInModelDb();
                lambdaUpdate(dbConnectionName(), pathToFolder, V2TableModelFileMonitor::TableItemStatus::Deleted);
            }
        }
    });

    watcher->setFuture(future);
}

void TabFileMonitor::slotOnFolderMoved(const QString &pathToFolder, const QString &oldFolderName)
{
    QDir parentDir(pathToFolder);
    parentDir.cdUp();
    QString pathToOldFolder = QDir::toNativeSeparators(parentDir.absolutePath()) + QDir::separator();
    pathToOldFolder += oldFolderName + QDir::separator();

    auto *watcher = new QFutureWatcher<void>(this);
    newResultSet.insert(watcher);
    QObject::connect(watcher, &QFutureWatcher<void>::finished,
                     this, &TabFileMonitor::slotRefreshTableViewFileMonitor);

    QFuture<void> future = QtConcurrent::run([=]{

        std::function<bool (QString)> lambdaIsExistInDb;
        lambdaIsExistInDb = LambdaFactoryTabFileMonitor::lambdaIsFolderExistInDb();

        std::function<bool (QString, QString)> lambdaIsExistInModelDb;
        lambdaIsExistInModelDb = LambdaFactoryTabFileMonitor::lambdaIsRowExistInModelDb();

        std::function<void (QString, QString, V2TableModelFileMonitor::TableItemStatus)> lambdaUpdate;
        lambdaUpdate = LambdaFactoryTabFileMonitor::lambdaUpdateStatusOfRowInModelDb();

        std::function<void (QString, QString, V2TableModelFileMonitor::TableItemStatus)> lambdaInsert;
        lambdaInsert = LambdaFactoryTabFileMonitor::lambdaInsertRowIntoModelDb();

        std::function<void (QString, QString, QString)> lambdaUpdateOldName;
        lambdaUpdateOldName = LambdaFactoryTabFileMonitor::lambdaUpdateOldNameOfRowInModelDb();

        std::function<void (QString, QString, QString)> lambdaUpdateName;
        lambdaUpdateName = LambdaFactoryTabFileMonitor::lambdaUpdateNameOfRowInModelDb();

        bool isOldFolderExistInDb = lambdaIsExistInDb(pathToOldFolder);
        bool isOldFolderExistInModelDb = lambdaIsExistInModelDb(dbConnectionName(), pathToOldFolder);

        if(isOldFolderExistInDb)
        {
            if(isOldFolderExistInModelDb)
                lambdaUpdate(dbConnectionName(), pathToOldFolder, V2TableModelFileMonitor::TableItemStatus::Moved);
            else
                lambdaInsert(dbConnectionName(), pathToOldFolder, V2TableModelFileMonitor::TableItemStatus::Moved);

            lambdaUpdateOldName(dbConnectionName(), pathToOldFolder, oldFolderName + QDir::separator());
        }

        lambdaUpdateName(dbConnectionName(), pathToOldFolder, QFileInfo(pathToFolder).dir().dirName() + QDir::separator());

        bool isNewFolderExistInDb = lambdaIsExistInDb(pathToFolder);

        if(isNewFolderExistInDb) // Is Folder renamed to original name
        {
            std::function<void (QString, QString)> lambdaDelete = LambdaFactoryTabFileMonitor::lambdaDeleteRowFromModelDb();
            lambdaDelete(dbConnectionName(), pathToFolder);
        }
    });

    watcher->setFuture(future);
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
        lambdaIsExistInModelDb = LambdaFactoryTabFileMonitor::lambdaIsRowExistInModelDb();
        bool isExistInModelDb = lambdaIsExistInModelDb(dbConnectionName(), pathToFile);

        if(isExistInDb)
        {
            if(isExistInModelDb)
            {
                std::function<void (QString, QString, V2TableModelFileMonitor::TableItemStatus)> lambdaUpdate;
                lambdaUpdate = LambdaFactoryTabFileMonitor::lambdaUpdateStatusOfRowInModelDb();
                lambdaUpdate(dbConnectionName(), pathToFile, V2TableModelFileMonitor::TableItemStatus::Modified);
            }
            else
            {
                std::function<void (QString, QString, V2TableModelFileMonitor::TableItemStatus)> lambdaInsert;
                lambdaInsert = LambdaFactoryTabFileMonitor::lambdaInsertRowIntoModelDb();
                lambdaInsert(dbConnectionName(), pathToFile, V2TableModelFileMonitor::TableItemStatus::Modified);
            }
        }
        else
        {
            if(isExistInModelDb)
            {
                std::function<void (QString, QString, V2TableModelFileMonitor::TableItemStatus)> lambdaUpdate;
                lambdaUpdate = LambdaFactoryTabFileMonitor::lambdaUpdateStatusOfRowInModelDb();
                lambdaUpdate(dbConnectionName(), pathToFile, V2TableModelFileMonitor::TableItemStatus::NewAdded);
            }
            else
            {
                std::function<void (QString, QString, V2TableModelFileMonitor::TableItemStatus)> lambdaInsert;
                lambdaInsert = LambdaFactoryTabFileMonitor::lambdaInsertRowIntoModelDb();
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
        lambdaIsExistInModelDb = LambdaFactoryTabFileMonitor::lambdaIsRowExistInModelDb();
        bool isExistInModelDb = lambdaIsExistInModelDb(dbConnectionName(), pathToFile);

        if(isExistInDb)
        {
            if(isExistInModelDb)
            {
                std::function<void (QString, QString, V2TableModelFileMonitor::TableItemStatus)> lambdaUpdate;
                lambdaUpdate = LambdaFactoryTabFileMonitor::lambdaUpdateStatusOfRowInModelDb();
                lambdaUpdate(dbConnectionName(), pathToFile, V2TableModelFileMonitor::TableItemStatus::Deleted);
            }
            else
            {
                std::function<void (QString, QString, V2TableModelFileMonitor::TableItemStatus)> lambdaInsert;
                lambdaInsert = LambdaFactoryTabFileMonitor::lambdaInsertRowIntoModelDb();
                lambdaInsert(dbConnectionName(), pathToFile, V2TableModelFileMonitor::TableItemStatus::Deleted);
            }
        }
        else
        {
            if(isExistInModelDb)
            {
                std::function<void (QString, QString, V2TableModelFileMonitor::TableItemStatus)> lambdaUpdate;
                lambdaUpdate = LambdaFactoryTabFileMonitor::lambdaUpdateStatusOfRowInModelDb();
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

        std::function<bool (QString)> lambdaIsOldFileExistInDb;
        lambdaIsOldFileExistInDb = LambdaFactoryTabFileMonitor::lambdaIsFileExistInDb();
        bool isOldFileExistInDb = lambdaIsOldFileExistInDb(pathToOldFile);

        std::function<bool (QString, QString)> lambdaIsOldFileExistInModelDb;
        lambdaIsOldFileExistInModelDb = LambdaFactoryTabFileMonitor::lambdaIsRowExistInModelDb();
        bool isOldFileExistInModelDb = lambdaIsOldFileExistInModelDb(dbConnectionName(), pathToOldFile);

        if(isOldFileExistInDb)
        {
            std::function<V2TableModelFileMonitor::TableItemStatus (QString, QString)> lambdaFetchStatus;
            lambdaFetchStatus = LambdaFactoryTabFileMonitor::lambdaFetchStatusOfRowFromModelDb();
            auto currentStatus = lambdaFetchStatus(dbConnectionName(), pathToOldFile);

            if(isOldFileExistInModelDb)
            {
                std::function<void (QString, QString, V2TableModelFileMonitor::TableItemStatus)> lambdaUpdate;
                lambdaUpdate = LambdaFactoryTabFileMonitor::lambdaUpdateStatusOfRowInModelDb();

                if(currentStatus == V2TableModelFileMonitor::TableItemStatus::Modified)
                    lambdaUpdate(dbConnectionName(), pathToOldFile, V2TableModelFileMonitor::TableItemStatus::MovedAndModified);
                else
                    lambdaUpdate(dbConnectionName(), pathToOldFile, V2TableModelFileMonitor::TableItemStatus::Moved);
            }
            else
            {
                std::function<void (QString, QString, V2TableModelFileMonitor::TableItemStatus)> lambdaInsert;
                lambdaInsert = LambdaFactoryTabFileMonitor::lambdaInsertRowIntoModelDb();

                if(currentStatus == V2TableModelFileMonitor::TableItemStatus::Modified)
                    lambdaInsert(dbConnectionName(), pathToOldFile, V2TableModelFileMonitor::TableItemStatus::MovedAndModified);
                else
                    lambdaInsert(dbConnectionName(), pathToOldFile, V2TableModelFileMonitor::TableItemStatus::Moved);
            }

            std::function<void (QString, QString, QString)> lambdaUpdateOldName;
            lambdaUpdateOldName = LambdaFactoryTabFileMonitor::lambdaUpdateOldNameOfRowInModelDb();
            lambdaUpdateOldName(dbConnectionName(), pathToOldFile, oldFileName);
        }

        std::function<void (QString, QString, QString)> lambdaUpdateName;
        lambdaUpdateName = LambdaFactoryTabFileMonitor::lambdaUpdateNameOfRowInModelDb();
        lambdaUpdateName(dbConnectionName(), pathToOldFile, QFileInfo(pathToFile).fileName());

        std::function<bool (QString)> lambdaIsNewFileExistInDb;
        lambdaIsNewFileExistInDb = LambdaFactoryTabFileMonitor::lambdaIsFileExistInDb();
        bool isNewFileExistInDb = lambdaIsNewFileExistInDb(pathToFile);

        if(isNewFileExistInDb)
        {
            std::function<void (QString, QString, QString)> lambdaUpdateOldName;
            lambdaUpdateOldName = LambdaFactoryTabFileMonitor::lambdaUpdateOldNameOfRowInModelDb();
            lambdaUpdateOldName(dbConnectionName(), pathToFile, "");

            std::function<V2TableModelFileMonitor::TableItemStatus (QString, QString)> lambdaFetchStatus;
            lambdaFetchStatus = LambdaFactoryTabFileMonitor::lambdaFetchStatusOfRowFromModelDb();
            auto currentStatus = lambdaFetchStatus(dbConnectionName(), pathToFile);

            if(currentStatus == V2TableModelFileMonitor::TableItemStatus::Moved) // File renamed as same again
            {
                std::function<void (QString, QString)> lambdaDelete = LambdaFactoryTabFileMonitor::lambdaDeleteRowFromModelDb();
                lambdaDelete(dbConnectionName(), pathToFile);
            }
            else if(currentStatus == V2TableModelFileMonitor::TableItemStatus::Deleted)
            {
                std::function<void (QString, QString, V2TableModelFileMonitor::TableItemStatus)> lambdaUpdate;
                lambdaUpdate = LambdaFactoryTabFileMonitor::lambdaUpdateStatusOfRowInModelDb();
                lambdaUpdate(dbConnectionName(), pathToFile, V2TableModelFileMonitor::TableItemStatus::Modified);

                std::function<void (QString, QString)> lambdaDelete = LambdaFactoryTabFileMonitor::lambdaDeleteRowFromModelDb();
                lambdaDelete(dbConnectionName(), pathToOldFile);
            }
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

        std::function<bool (QString, QString)> lambdaIsExistInModelDb;
        lambdaIsExistInModelDb = LambdaFactoryTabFileMonitor::lambdaIsRowExistInModelDb();
        bool isExistInModelDb = lambdaIsExistInModelDb(dbConnectionName(), pathToFile);

        std::function<V2TableModelFileMonitor::TableItemStatus (QString, QString)> lambdaFetchStatus;
        lambdaFetchStatus = LambdaFactoryTabFileMonitor::lambdaFetchStatusOfRowFromModelDb();
        auto currentStatus = lambdaFetchStatus(dbConnectionName(), pathToFile);

        if(isExistInDb)
        {
            if(isExistInModelDb)
            {
                std::function<void (QString, QString, V2TableModelFileMonitor::TableItemStatus)> lambdaUpdate;
                lambdaUpdate = LambdaFactoryTabFileMonitor::lambdaUpdateStatusOfRowInModelDb();

                if(currentStatus == V2TableModelFileMonitor::Moved ||
                   currentStatus == V2TableModelFileMonitor::TableItemStatus::MovedAndModified)
                    lambdaUpdate(dbConnectionName(), pathToFile, V2TableModelFileMonitor::TableItemStatus::MovedAndModified);
                else
                    lambdaUpdate(dbConnectionName(), pathToFile, V2TableModelFileMonitor::TableItemStatus::Modified);
            }
            else
            {
                std::function<void (QString, QString, V2TableModelFileMonitor::TableItemStatus)> lambdaInsert;
                lambdaInsert = LambdaFactoryTabFileMonitor::lambdaInsertRowIntoModelDb();
                lambdaInsert(dbConnectionName(), pathToFile, V2TableModelFileMonitor::TableItemStatus::Modified);
            }
        }
        else
        {
            if(isExistInModelDb)
            {
                if(currentStatus == V2TableModelFileMonitor::Moved)
                {
                    std::function<void (QString, QString, V2TableModelFileMonitor::TableItemStatus)> lambdaUpdate;
                    lambdaUpdate = LambdaFactoryTabFileMonitor::lambdaUpdateStatusOfRowInModelDb();
                    lambdaUpdate(dbConnectionName(), pathToFile, V2TableModelFileMonitor::TableItemStatus::MovedAndModified);
                }
            }
        }
    });

    watcher->setFuture(future);
}

void TabFileMonitor::slotOnFileMovedAndModified(const QString &pathToFile, const QString &oldFileName)
{
    QString pathToOldFile = QDir::toNativeSeparators(QFileInfo(pathToFile).absolutePath()) + QDir::separator();
    pathToOldFile += oldFileName;

    auto *watcher = new QFutureWatcher<void>(this);
    newResultSet.insert(watcher);
    QObject::connect(watcher, &QFutureWatcher<void>::finished,
                     this, &TabFileMonitor::slotRefreshTableViewFileMonitor);

    QFuture<void> future = QtConcurrent::run([=]{

        std::function<bool (QString)> lambdaIsOldFileExistInDb = LambdaFactoryTabFileMonitor::lambdaIsFileExistInDb();
        bool isOldFileExistInDb = lambdaIsOldFileExistInDb(pathToOldFile);

        std::function<bool (QString, QString)> lambdaIsOldFileExistInModelDb;
        lambdaIsOldFileExistInModelDb = LambdaFactoryTabFileMonitor::lambdaIsRowExistInModelDb();
        bool isOldFileExistInModelDb = lambdaIsOldFileExistInModelDb(dbConnectionName(), pathToOldFile);

        if(isOldFileExistInDb)
        {
            if(isOldFileExistInModelDb)
            {
                std::function<void (QString, QString, V2TableModelFileMonitor::TableItemStatus)> lambdaUpdate;
                lambdaUpdate = LambdaFactoryTabFileMonitor::lambdaUpdateStatusOfRowInModelDb();
                lambdaUpdate(dbConnectionName(), pathToOldFile, V2TableModelFileMonitor::TableItemStatus::MovedAndModified);
            }
            else
            {
                std::function<void (QString, QString, V2TableModelFileMonitor::TableItemStatus)> lambdaInsert;
                lambdaInsert = LambdaFactoryTabFileMonitor::lambdaInsertRowIntoModelDb();
                lambdaInsert(dbConnectionName(), pathToOldFile, V2TableModelFileMonitor::TableItemStatus::MovedAndModified);
            }

            std::function<void (QString, QString, QString)> lambdaUpdateOldName;
            lambdaUpdateOldName = LambdaFactoryTabFileMonitor::lambdaUpdateOldNameOfRowInModelDb();
            lambdaUpdateOldName(dbConnectionName(), pathToOldFile, oldFileName);
        }

        std::function<void (QString, QString, QString)> lambdaUpdateName;
        lambdaUpdateName = LambdaFactoryTabFileMonitor::lambdaUpdateNameOfRowInModelDb();
        lambdaUpdateName(dbConnectionName(), pathToOldFile, QFileInfo(pathToFile).fileName());

        std::function<bool (QString)> lambdaIsNewFileExistInDb;
        lambdaIsNewFileExistInDb = LambdaFactoryTabFileMonitor::lambdaIsFileExistInDb();
        bool isNewFileExistInDb = lambdaIsNewFileExistInDb(pathToFile);

        if(isNewFileExistInDb) // File renamed as same again
        {
            std::function<void (QString, QString, V2TableModelFileMonitor::TableItemStatus)> lambdaUpdate;
            lambdaUpdate = LambdaFactoryTabFileMonitor::lambdaUpdateStatusOfRowInModelDb();
            lambdaUpdate(dbConnectionName(), pathToFile, V2TableModelFileMonitor::TableItemStatus::Modified);

            std::function<void (QString, QString, QString)> lambdaUpdateOldName;
            lambdaUpdateOldName = LambdaFactoryTabFileMonitor::lambdaUpdateOldNameOfRowInModelDb();
            lambdaUpdateOldName(dbConnectionName(), pathToFile, "");
        }
    });

    watcher->setFuture(future);
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
