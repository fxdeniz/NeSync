#include "TabFileMonitor.h"
#include "ui_TabFileMonitor.h"
#include "Tasks/LambdaFactoryTabFileMonitor.h"
#include "DataModels/TabFileMonitor/TableModelFileMonitor.h"

#include <QtConcurrent>
#include <QSqlQuery>
#include <QFileInfo>
#include <QDir>

TabFileMonitor::TabFileMonitor(QWidget *parent) :
    IComboBoxNoteNotifier(parent),
    ui(new Ui::TabFileMonitor)
{
    ui->setupUi(this);
    createDb();

    ui->comboBoxNoteNumber->setPlaceholderText(defaultNoNoteText());

    this->comboBoxItemDelegateNote = new ComboBoxItemDelegateNote(this);
    this->comboBoxItemDelegateFileAction = new ComboBoxItemDelegateFileAction(this);
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

void TabFileMonitor::slotOnActionSaveAllTriggered()
{
    auto *savingWatcher = new QFutureWatcher<void>(this);
    resultSet.insert(savingWatcher);
    QObject::connect(savingWatcher, &QFutureWatcher<void>::finished,
                     this, &TabFileMonitor::slotOnAsyncTaskCompleted);

    QFuture<void> savingFuture = QtConcurrent::run([=]{

        QStringList folderPathList = LambdaFactoryTabFileMonitor::fetchRowsByProgressFromModelDb()(dbConnectionName(),
                                                                                                TableModelFileMonitor::ItemType::Folder,
                                                                                                TableModelFileMonitor::ProgressStatus::WaitingForUserInteraction);
         for(const QString &item : folderPathList)
         {
             bool isApplied = LambdaFactoryTabFileMonitor::applyActionForFolder()(dbConnectionName(), item);

             if(isApplied)
             {
                 LambdaFactoryTabFileMonitor::updateProgressOfRowInModelDb()(dbConnectionName(),
                                                                             item,
                                                                             TableModelFileMonitor::ProgressStatus::Completed);
                 LambdaFactoryTabFileMonitor::deleteRowFromModelDb()(dbConnectionName(), item);
             }
             else
                 LambdaFactoryTabFileMonitor::updateProgressOfRowInModelDb()(dbConnectionName(),
                                                                             item,
                                                                             TableModelFileMonitor::ProgressStatus::ErrorOccured);
        }
    }).then(QtFuture::Launch::Inherit, [=]{

        QStringList filePathList = LambdaFactoryTabFileMonitor::fetchRowsByProgressFromModelDb()(dbConnectionName(),
                                                                                                 TableModelFileMonitor::ItemType::File,
                                                                                                 TableModelFileMonitor::WaitingForUserInteraction);
        for(const QString &item : filePathList)
        {
            bool isApplied = LambdaFactoryTabFileMonitor::applyActionForFile()(dbConnectionName(), item);

            if(isApplied)
            {
                LambdaFactoryTabFileMonitor::updateProgressOfRowInModelDb()(dbConnectionName(),
                                                                            item,
                                                                            TableModelFileMonitor::ProgressStatus::Completed);
                LambdaFactoryTabFileMonitor::deleteRowFromModelDb()(dbConnectionName(), item);
            }
            else
                LambdaFactoryTabFileMonitor::updateProgressOfRowInModelDb()(dbConnectionName(),
                                                                            item,
                                                                            TableModelFileMonitor::ProgressStatus::ErrorOccured);
        }
    });

    savingWatcher->setFuture(savingFuture);
}

void TabFileMonitor::slotOnPredictionTargetNotFound(const QString &pathToFileOrFolder)
{
    auto *watcher = new QFutureWatcher<void>(this);
    resultSet.insert(watcher);
    QObject::connect(watcher, &QFutureWatcher<void>::finished,
                     this, &TabFileMonitor::slotOnAsyncTaskCompleted);

    QFuture<void> future = QtConcurrent::run([=]{
        std::function<void (QString, QString, TableModelFileMonitor::ItemStatus)> lambdaInsert;
        lambdaInsert = LambdaFactoryTabFileMonitor::insertRowIntoModelDb();
        lambdaInsert(dbConnectionName(), pathToFileOrFolder, TableModelFileMonitor::ItemStatus::Missing);
    });

    watcher->setFuture(future);
}

void TabFileMonitor::slotOnUnPredictedFolderDetected(const QString &pathToFolder)
{
    auto *watcher = new QFutureWatcher<void>(this);
    resultSet.insert(watcher);
    QObject::connect(watcher, &QFutureWatcher<void>::finished,
                     this, &TabFileMonitor::slotOnAsyncTaskCompleted);

    QFuture<void> future = QtConcurrent::run([=]{

        std::function<void (QString, QString, TableModelFileMonitor::ItemStatus)> lambdaInsert;
        lambdaInsert = LambdaFactoryTabFileMonitor::insertRowIntoModelDb();
        lambdaInsert(dbConnectionName(), pathToFolder, TableModelFileMonitor::ItemStatus::NewAdded);
    });

    watcher->setFuture(future);
}

void TabFileMonitor::slotOnNewFolderAdded(const QString &pathToFolder)
{
    auto *watcher = new QFutureWatcher<void>(this);
    resultSet.insert(watcher);
    QObject::connect(watcher, &QFutureWatcher<void>::finished,
                     this, &TabFileMonitor::slotOnAsyncTaskCompleted);

    QFuture<void> future = QtConcurrent::run([=]{

        std::function<bool (QString)> lambdaIsExistInDb;
        lambdaIsExistInDb = LambdaFactoryTabFileMonitor::isFolderExistInDb();

        std::function<bool (QString, QString)> lambdaIsExistInModelDb;
        lambdaIsExistInModelDb = LambdaFactoryTabFileMonitor::isRowExistInModelDb();

        std::function<void (QString, QString, TableModelFileMonitor::ItemStatus)> lambdaUpdateStatus;
        lambdaUpdateStatus = LambdaFactoryTabFileMonitor::updateStatusOfRowInModelDb();

        std::function<void (QString, QString, TableModelFileMonitor::ItemStatus)> lambdaInsert;
        lambdaInsert = LambdaFactoryTabFileMonitor::insertRowIntoModelDb();

        bool isExistInDb = lambdaIsExistInDb(pathToFolder);
        bool isExistInModelDb = lambdaIsExistInModelDb(dbConnectionName(), pathToFolder);

        if(isExistInDb)
        {
            if(isExistInModelDb)
                lambdaUpdateStatus(dbConnectionName(), pathToFolder, TableModelFileMonitor::ItemStatus::Modified);
            else
                lambdaInsert(dbConnectionName(), pathToFolder, TableModelFileMonitor::ItemStatus::Modified);
        }
        else
        {
            if(isExistInModelDb)
                lambdaUpdateStatus(dbConnectionName(), pathToFolder, TableModelFileMonitor::ItemStatus::NewAdded);
            else
                lambdaInsert(dbConnectionName(), pathToFolder, TableModelFileMonitor::ItemStatus::NewAdded);
        }
    });

    watcher->setFuture(future);
}

void TabFileMonitor::slotOnFolderDeleted(const QString &pathToFolder)
{
    auto *watcher = new QFutureWatcher<void>(this);
    resultSet.insert(watcher);
    QObject::connect(watcher, &QFutureWatcher<void>::finished,
                     this, &TabFileMonitor::slotOnAsyncTaskCompleted);

    QFuture<void> future = QtConcurrent::run([=]{

        std::function<bool (QString)> lambdaIsExistInDb;
        lambdaIsExistInDb = LambdaFactoryTabFileMonitor::isFolderExistInDb();

        std::function<bool (QString, QString)> lambdaIsExistInModelDb;
        lambdaIsExistInModelDb = LambdaFactoryTabFileMonitor::isRowExistInModelDb();

        std::function<void (QString, QString, TableModelFileMonitor::ItemStatus)> lambdaInsert;
        lambdaInsert = LambdaFactoryTabFileMonitor::insertRowIntoModelDb();

        std::function<void (QString, QString, TableModelFileMonitor::ItemStatus)> lambdaUpdateStatus;
        lambdaUpdateStatus = LambdaFactoryTabFileMonitor::updateStatusOfRowInModelDb();


        bool isExistInDb = lambdaIsExistInDb(pathToFolder);
        bool isExistInModelDb = lambdaIsExistInModelDb(dbConnectionName(), pathToFolder);

        if(isExistInDb)
        {
            if(isExistInModelDb)
                lambdaUpdateStatus(dbConnectionName(), pathToFolder, TableModelFileMonitor::ItemStatus::Deleted);
            else
                lambdaInsert(dbConnectionName(), pathToFolder, TableModelFileMonitor::ItemStatus::Deleted);
        }
        else
        {
            if(isExistInModelDb)
                lambdaUpdateStatus(dbConnectionName(), pathToFolder, TableModelFileMonitor::ItemStatus::Deleted);
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

    auto *categorizationWatcher = new QFutureWatcher<void>(this);
    resultSet.insert(categorizationWatcher);
    QObject::connect(categorizationWatcher, &QFutureWatcher<void>::finished,
                     this, &TabFileMonitor::slotOnAsyncTaskCompleted);

    QFuture<void> categorizationFuture = QtConcurrent::run([=]{

        std::function<bool (QString)> lambdaIsExistInDb;
        lambdaIsExistInDb = LambdaFactoryTabFileMonitor::isFolderExistInDb();

        std::function<bool (QString, QString)> lambdaIsExistInModelDb;
        lambdaIsExistInModelDb = LambdaFactoryTabFileMonitor::isRowExistInModelDb();

        std::function<void (QString, QString, TableModelFileMonitor::ItemStatus)> lambdaUpdateStatus;
        lambdaUpdateStatus = LambdaFactoryTabFileMonitor::updateStatusOfRowInModelDb();

        std::function<void (QString, QString, TableModelFileMonitor::ItemStatus)> lambdaInsert;
        lambdaInsert = LambdaFactoryTabFileMonitor::insertRowIntoModelDb();

        std::function<void (QString, QString, QString)> lambdaUpdateOldName;
        lambdaUpdateOldName = LambdaFactoryTabFileMonitor::updateOldNameOfRowInModelDb();

        std::function<void (QString, QString, QString)> lambdaUpdateName;
        lambdaUpdateName = LambdaFactoryTabFileMonitor::updateNameOfRowInModelDb();

        bool isOldFolderExistInDb = lambdaIsExistInDb(pathToOldFolder);
        bool isOldFolderExistInModelDb = lambdaIsExistInModelDb(dbConnectionName(), pathToOldFolder);

        if(isOldFolderExistInDb)
        {
            if(isOldFolderExistInModelDb)
                lambdaUpdateStatus(dbConnectionName(), pathToOldFolder, TableModelFileMonitor::ItemStatus::Moved);
            else
                lambdaInsert(dbConnectionName(), pathToOldFolder, TableModelFileMonitor::ItemStatus::Moved);

            lambdaUpdateOldName(dbConnectionName(), pathToOldFolder, oldFolderName + QDir::separator());
        }

        lambdaUpdateName(dbConnectionName(), pathToOldFolder, QFileInfo(pathToFolder).dir().dirName() + QDir::separator());

        bool isNewFolderExistInDb = lambdaIsExistInDb(pathToFolder);

        if(isNewFolderExistInDb) // Is Folder renamed to original name
        {
            std::function<void (QString, QString)> lambdaDelete = LambdaFactoryTabFileMonitor::deleteRowFromModelDb();
            lambdaDelete(dbConnectionName(), pathToFolder);
        }
    });

    auto *savingWatcher = new QFutureWatcher<void>(this);
    resultSet.insert(savingWatcher);
    QObject::connect(savingWatcher, &QFutureWatcher<void>::finished,
                     this, &TabFileMonitor::slotOnAsyncTaskCompleted);

    QFuture<void> savingFuture = categorizationFuture.then(QtFuture::Launch::Inherit, [=]{

        QThread::currentThread()->usleep(1000000); // Give some rome for categorizationFuture's result to be displayed.
        QStringList filePathList = LambdaFactoryTabFileMonitor::fetchRowsByProgressFromModelDb()(dbConnectionName(),
                                                                                                 TableModelFileMonitor::ItemType::Folder,
                                                                                                 TableModelFileMonitor::ProgressStatus::ApplyingAutoAction);

        for(const QString &item : filePathList)
        {
            bool isSaved = LambdaFactoryTabFileMonitor::applyActionForFolder()(dbConnectionName(), item);

            if(isSaved)
            {
                LambdaFactoryTabFileMonitor::updateProgressOfRowInModelDb()(dbConnectionName(),
                                                                            item,
                                                                            TableModelFileMonitor::ProgressStatus::Completed);
                LambdaFactoryTabFileMonitor::deleteRowFromModelDb()(dbConnectionName(), item);
            }
            else
                LambdaFactoryTabFileMonitor::updateProgressOfRowInModelDb()(dbConnectionName(),
                                                                            item,
                                                                            TableModelFileMonitor::ProgressStatus::ErrorOccured);
        }
    });


    categorizationWatcher->setFuture(categorizationFuture);
    savingWatcher->setFuture(savingFuture);

}

void TabFileMonitor::slotOnUnPredictedFileDetected(const QString &pathToFile)
{
    auto *watcher = new QFutureWatcher<void>(this);
    resultSet.insert(watcher);
    QObject::connect(watcher, &QFutureWatcher<void>::finished,
                     this, &TabFileMonitor::slotOnAsyncTaskCompleted);

    QFuture<void> future = QtConcurrent::run([=]{

        std::function<void (QString, QString, TableModelFileMonitor::ItemStatus)> lambdaInsert;
        lambdaInsert = LambdaFactoryTabFileMonitor::insertRowIntoModelDb();
        lambdaInsert(dbConnectionName(), pathToFile, TableModelFileMonitor::ItemStatus::NewAdded);
    });

    watcher->setFuture(future);
}

void TabFileMonitor::slotOnNewFileAdded(const QString &pathToFile)
{
    auto *watcher = new QFutureWatcher<void>(this);
    resultSet.insert(watcher);
    QObject::connect(watcher, &QFutureWatcher<void>::finished,
                     this, &TabFileMonitor::slotOnAsyncTaskCompleted);

    QFuture<void> future = QtConcurrent::run([=]{

        std::function<bool (QString)> lambdaIsExistInDb;
        lambdaIsExistInDb = LambdaFactoryTabFileMonitor::isFileExistInDb();

        std::function<bool (QString, QString)> lambdaIsExistInModelDb;
        lambdaIsExistInModelDb = LambdaFactoryTabFileMonitor::isRowExistInModelDb();

        std::function<void (QString, QString, TableModelFileMonitor::ItemStatus)> lambdaInsert;
        lambdaInsert = LambdaFactoryTabFileMonitor::insertRowIntoModelDb();

        std::function<void (QString, QString, TableModelFileMonitor::ItemStatus)> lambdaUpdateStatus;
        lambdaUpdateStatus = LambdaFactoryTabFileMonitor::updateStatusOfRowInModelDb();

        bool isExistInDb = lambdaIsExistInDb(pathToFile);
        bool isExistInModelDb = lambdaIsExistInModelDb(dbConnectionName(), pathToFile);

        if(isExistInDb)
        {
            if(isExistInModelDb)
                lambdaUpdateStatus(dbConnectionName(), pathToFile, TableModelFileMonitor::ItemStatus::Modified);
            else
                lambdaInsert(dbConnectionName(), pathToFile, TableModelFileMonitor::ItemStatus::Modified);
        }
        else
        {
            if(isExistInModelDb)
                lambdaUpdateStatus(dbConnectionName(), pathToFile, TableModelFileMonitor::ItemStatus::NewAdded);
            else
                lambdaInsert(dbConnectionName(), pathToFile, TableModelFileMonitor::ItemStatus::NewAdded);
        }
    });

    watcher->setFuture(future);
}

void TabFileMonitor::slotOnFileDeleted(const QString &pathToFile)
{
    auto *watcher = new QFutureWatcher<void>(this);
    resultSet.insert(watcher);
    QObject::connect(watcher, &QFutureWatcher<void>::finished,
                     this, &TabFileMonitor::slotOnAsyncTaskCompleted);

    QFuture<void> future = QtConcurrent::run([=]{

        std::function<bool (QString)> lambdaIsExistInDb;
        lambdaIsExistInDb = LambdaFactoryTabFileMonitor::isFileExistInDb();

        std::function<bool (QString, QString)> lambdaIsExistInModelDb;
        lambdaIsExistInModelDb = LambdaFactoryTabFileMonitor::isRowExistInModelDb();

        std::function<void (QString, QString, TableModelFileMonitor::ItemStatus)> lambdaInsert;
        lambdaInsert = LambdaFactoryTabFileMonitor::insertRowIntoModelDb();

        std::function<void (QString, QString, TableModelFileMonitor::ItemStatus)> lambdaUpdateStaus;
        lambdaUpdateStaus = LambdaFactoryTabFileMonitor::updateStatusOfRowInModelDb();

        bool isExistInDb = lambdaIsExistInDb(pathToFile);
        bool isExistInModelDb = lambdaIsExistInModelDb(dbConnectionName(), pathToFile);

        if(isExistInDb)
        {
            if(isExistInModelDb)
                lambdaUpdateStaus(dbConnectionName(), pathToFile, TableModelFileMonitor::ItemStatus::Deleted);
            else
                lambdaInsert(dbConnectionName(), pathToFile, TableModelFileMonitor::ItemStatus::Deleted);
        }
        else
        {
            if(isExistInModelDb)
                lambdaUpdateStaus(dbConnectionName(), pathToFile, TableModelFileMonitor::ItemStatus::Deleted);
        }
    });

    watcher->setFuture(future);
}

void TabFileMonitor::slotOnFileMoved(const QString &pathToFile, const QString &oldFileName)
{
    QString pathToOldFile = QDir::toNativeSeparators(QFileInfo(pathToFile).absolutePath()) + QDir::separator();
    pathToOldFile += oldFileName;

    auto *categorizationWatcher = new QFutureWatcher<void>(this);
    resultSet.insert(categorizationWatcher);
    QObject::connect(categorizationWatcher, &QFutureWatcher<void>::finished,
                     this, &TabFileMonitor::slotOnAsyncTaskCompleted);

    QFuture<void> categorizationFuture = QtConcurrent::run([=]{

        std::function<bool (QString)> lambdaIsdFileExist;
        lambdaIsdFileExist = LambdaFactoryTabFileMonitor::isFileExistInDb();

        std::function<bool (QString, QString)> lambdaIsOldFileExistInModelDb;
        lambdaIsOldFileExistInModelDb = LambdaFactoryTabFileMonitor::isRowExistInModelDb();

        std::function<TableModelFileMonitor::ItemStatus (QString, QString)> lambdaFetchStatus;
        lambdaFetchStatus = LambdaFactoryTabFileMonitor::fetchStatusOfRowFromModelDb();

        std::function<void (QString, QString, TableModelFileMonitor::ItemStatus)> lambdaInsert;
        lambdaInsert = LambdaFactoryTabFileMonitor::insertRowIntoModelDb();

        std::function<void (QString, QString, TableModelFileMonitor::ItemStatus)> lambdaUpdateStatus;
        lambdaUpdateStatus = LambdaFactoryTabFileMonitor::updateStatusOfRowInModelDb();

        std::function<void (QString, QString, QString)> lambdaUpdateOldName;
        lambdaUpdateOldName = LambdaFactoryTabFileMonitor::updateOldNameOfRowInModelDb();

        std::function<void (QString, QString, QString)> lambdaUpdateName;
        lambdaUpdateName = LambdaFactoryTabFileMonitor::updateNameOfRowInModelDb();

        std::function<void (QString, QString)> lambdaDeleteRow;
        lambdaDeleteRow = LambdaFactoryTabFileMonitor::deleteRowFromModelDb();


        bool isOldFileExistInDb = lambdaIsdFileExist(pathToOldFile);
        bool isOldFileExistInModelDb = lambdaIsOldFileExistInModelDb(dbConnectionName(), pathToOldFile);

        if(isOldFileExistInDb)
        {
            auto currentStatus = lambdaFetchStatus(dbConnectionName(), pathToOldFile);

            if(isOldFileExistInModelDb)
            {
                if(currentStatus == TableModelFileMonitor::ItemStatus::Modified)
                    lambdaUpdateStatus(dbConnectionName(), pathToOldFile, TableModelFileMonitor::ItemStatus::MovedAndModified);
                else
                    lambdaUpdateStatus(dbConnectionName(), pathToOldFile, TableModelFileMonitor::ItemStatus::Moved);
            }
            else
            {
                if(currentStatus == TableModelFileMonitor::ItemStatus::Modified)
                    lambdaInsert(dbConnectionName(), pathToOldFile, TableModelFileMonitor::ItemStatus::MovedAndModified);
                else
                    lambdaInsert(dbConnectionName(), pathToOldFile, TableModelFileMonitor::ItemStatus::Moved);
            }

            lambdaUpdateOldName(dbConnectionName(), pathToOldFile, oldFileName);
        }

        lambdaUpdateName(dbConnectionName(), pathToOldFile, QFileInfo(pathToFile).fileName());

        bool isNewFileExistInDb = lambdaIsdFileExist(pathToFile);

        if(isNewFileExistInDb)
        {
            lambdaUpdateOldName(dbConnectionName(), pathToFile, "");

            auto currentStatus = lambdaFetchStatus(dbConnectionName(), pathToFile);

            if(currentStatus == TableModelFileMonitor::ItemStatus::Moved) // File renamed as same again
            {
                lambdaDeleteRow(dbConnectionName(), pathToFile);
            }
            else if(currentStatus == TableModelFileMonitor::ItemStatus::Deleted)
            {
                lambdaUpdateStatus(dbConnectionName(), pathToFile, TableModelFileMonitor::ItemStatus::Modified);

                lambdaDeleteRow(dbConnectionName(), pathToOldFile);
            }
        }
    });

    auto *savingWatcher = new QFutureWatcher<void>(this);
    resultSet.insert(savingWatcher);
    QObject::connect(savingWatcher, &QFutureWatcher<void>::finished,
                     this, &TabFileMonitor::slotOnAsyncTaskCompleted);

    QFuture<void> savingFuture = categorizationFuture.then(QtFuture::Launch::Inherit, [=]{

        QThread::currentThread()->usleep(1000000); // Give some rome for categorizationFuture's result to be displayed.
        QStringList filePathList = LambdaFactoryTabFileMonitor::fetchRowsByProgressFromModelDb()(dbConnectionName(),
                                                                                                 TableModelFileMonitor::ItemType::File,
                                                                                                 TableModelFileMonitor::ProgressStatus::ApplyingAutoAction);

        for(const QString &item : filePathList)
        {
            bool isSaved = LambdaFactoryTabFileMonitor::applyActionForFile()(dbConnectionName(), item);

            if(isSaved)
            {
                LambdaFactoryTabFileMonitor::updateProgressOfRowInModelDb()(dbConnectionName(),
                                                                                  item,
                                                                                  TableModelFileMonitor::ProgressStatus::Completed);
                LambdaFactoryTabFileMonitor::deleteRowFromModelDb()(dbConnectionName(), item);
            }
            else
                LambdaFactoryTabFileMonitor::updateProgressOfRowInModelDb()(dbConnectionName(),
                                                                                  item,
                                                                                  TableModelFileMonitor::ProgressStatus::ErrorOccured);
        }
    });


    categorizationWatcher->setFuture(categorizationFuture);
    savingWatcher->setFuture(savingFuture);
}

void TabFileMonitor::slotOnFileModified(const QString &pathToFile)
{
    auto *categorizationWatcher = new QFutureWatcher<void>(this);
    resultSet.insert(categorizationWatcher);
    QObject::connect(categorizationWatcher, &QFutureWatcher<void>::finished,
                     this, &TabFileMonitor::slotOnAsyncTaskCompleted);

    QFuture<void> categorizationFuture = QtConcurrent::run([=]{

        std::function<bool (QString)> lambdaIsExistInDb;
        lambdaIsExistInDb = LambdaFactoryTabFileMonitor::isFileExistInDb();

        std::function<bool (QString, QString)> lambdaIsExistInModelDb;
        lambdaIsExistInModelDb = LambdaFactoryTabFileMonitor::isRowExistInModelDb();

        std::function<TableModelFileMonitor::ItemStatus (QString, QString)> lambdaFetchStatus;
        lambdaFetchStatus = LambdaFactoryTabFileMonitor::fetchStatusOfRowFromModelDb();

        std::function<void (QString, QString, TableModelFileMonitor::ItemStatus)> lambdaUpdateStatus;
        lambdaUpdateStatus = LambdaFactoryTabFileMonitor::updateStatusOfRowInModelDb();

        std::function<void (QString, QString, TableModelFileMonitor::ItemStatus)> lambdaInsert;
        lambdaInsert = LambdaFactoryTabFileMonitor::insertRowIntoModelDb();


        bool isExistInDb = lambdaIsExistInDb(pathToFile);
        bool isExistInModelDb = lambdaIsExistInModelDb(dbConnectionName(), pathToFile);

        auto currentStatus = lambdaFetchStatus(dbConnectionName(), pathToFile);

        if(isExistInDb)
        {
            if(isExistInModelDb)
            {

                if(currentStatus == TableModelFileMonitor::Moved ||
                   currentStatus == TableModelFileMonitor::ItemStatus::MovedAndModified)
                    lambdaUpdateStatus(dbConnectionName(), pathToFile, TableModelFileMonitor::ItemStatus::MovedAndModified);
                else
                    lambdaUpdateStatus(dbConnectionName(), pathToFile, TableModelFileMonitor::ItemStatus::Modified);
            }
            else
                lambdaInsert(dbConnectionName(), pathToFile, TableModelFileMonitor::ItemStatus::Modified);
        }
        else
        {
            if(isExistInModelDb)
            {
                if(currentStatus == TableModelFileMonitor::Moved)
                    lambdaUpdateStatus(dbConnectionName(), pathToFile, TableModelFileMonitor::ItemStatus::MovedAndModified);
            }
        }
    });

    auto *savingWatcher = new QFutureWatcher<void>(this);
    resultSet.insert(savingWatcher);
    QObject::connect(savingWatcher, &QFutureWatcher<void>::finished,
                     this, &TabFileMonitor::slotOnAsyncTaskCompleted);

    QFuture<void> savingFuture = categorizationFuture.then(QtFuture::Launch::Inherit, [=]{

        QThread::currentThread()->usleep(10000000); // Give some rome for categorizationFuture's result to be displayed.
        QStringList filePathList = LambdaFactoryTabFileMonitor::fetchRowsByProgressFromModelDb()(dbConnectionName(),
                                                                                                 TableModelFileMonitor::ItemType::File,
                                                                                                 TableModelFileMonitor::ProgressStatus::ApplyingAutoAction);

        for(const QString &item : filePathList)
        {
            bool isAdded = LambdaFactoryTabFileMonitor::applyActionForFile()(dbConnectionName(), item);

            if(isAdded)
            {
                LambdaFactoryTabFileMonitor::updateProgressOfRowInModelDb()(dbConnectionName(),
                                                                                  item,
                                                                                  TableModelFileMonitor::ProgressStatus::Completed);
                LambdaFactoryTabFileMonitor::deleteRowFromModelDb()(dbConnectionName(), item);
            }
            else
                LambdaFactoryTabFileMonitor::updateProgressOfRowInModelDb()(dbConnectionName(),
                                                                                  item,
                                                                                  TableModelFileMonitor::ProgressStatus::ErrorOccured);
        }
    });

    categorizationWatcher->setFuture(categorizationFuture);
    savingWatcher->setFuture(savingFuture);
}

void TabFileMonitor::slotOnFileMovedAndModified(const QString &pathToFile, const QString &oldFileName)
{
    QString pathToOldFile = QDir::toNativeSeparators(QFileInfo(pathToFile).absolutePath()) + QDir::separator();
    pathToOldFile += oldFileName;

    auto *categorizationWatcher = new QFutureWatcher<void>(this);
    resultSet.insert(categorizationWatcher);
    QObject::connect(categorizationWatcher, &QFutureWatcher<void>::finished,
                     this, &TabFileMonitor::slotOnAsyncTaskCompleted);

    QFuture<void> categorizationFuture = QtConcurrent::run([=]{

        std::function<bool (QString)> lambdaIsFileExistInDb = LambdaFactoryTabFileMonitor::isFileExistInDb();
        lambdaIsFileExistInDb = LambdaFactoryTabFileMonitor::isFileExistInDb();

        std::function<bool (QString, QString)> lambdaIsOldFileExistInModelDb;
        lambdaIsOldFileExistInModelDb = LambdaFactoryTabFileMonitor::isRowExistInModelDb();

        std::function<void (QString, QString, TableModelFileMonitor::ItemStatus)> lambdaInsert;
        lambdaInsert = LambdaFactoryTabFileMonitor::insertRowIntoModelDb();

        std::function<void (QString, QString, TableModelFileMonitor::ItemStatus)> lambdaUpdateStatus;
        lambdaUpdateStatus = LambdaFactoryTabFileMonitor::updateStatusOfRowInModelDb();

        std::function<void (QString, QString, QString)> lambdaUpdateOldName;
        lambdaUpdateOldName = LambdaFactoryTabFileMonitor::updateOldNameOfRowInModelDb();

        std::function<void (QString, QString, QString)> lambdaUpdateName;
        lambdaUpdateName = LambdaFactoryTabFileMonitor::updateNameOfRowInModelDb();


        bool isOldFileExistInDb = lambdaIsFileExistInDb(pathToOldFile);
        bool isOldFileExistInModelDb = lambdaIsOldFileExistInModelDb(dbConnectionName(), pathToOldFile);

        if(isOldFileExistInDb)
        {
            if(isOldFileExistInModelDb)
                lambdaUpdateStatus(dbConnectionName(), pathToOldFile, TableModelFileMonitor::ItemStatus::MovedAndModified);

            else
                lambdaInsert(dbConnectionName(), pathToOldFile, TableModelFileMonitor::ItemStatus::MovedAndModified);

            lambdaUpdateOldName(dbConnectionName(), pathToOldFile, oldFileName);
        }

        lambdaUpdateName(dbConnectionName(), pathToOldFile, QFileInfo(pathToFile).fileName());

        bool isNewFileExistInDb = lambdaIsFileExistInDb(pathToFile);

        if(isNewFileExistInDb) // File renamed as same again
        {
            lambdaUpdateStatus(dbConnectionName(), pathToFile, TableModelFileMonitor::ItemStatus::Modified);

            lambdaUpdateOldName(dbConnectionName(), pathToFile, "");
        }
    });

    auto *savingWatcher = new QFutureWatcher<void>(this);
    resultSet.insert(savingWatcher);
    QObject::connect(savingWatcher, &QFutureWatcher<void>::finished,
                     this, &TabFileMonitor::slotOnAsyncTaskCompleted);

    QFuture<void> savingFuture = categorizationFuture.then(QtFuture::Launch::Inherit, [=]{

        QThread::currentThread()->usleep(10000000); // Give some rome for categorizationFuture's result to be displayed.
        QStringList filePathList = LambdaFactoryTabFileMonitor::fetchRowsByProgressFromModelDb()(dbConnectionName(),
                                                                                                 TableModelFileMonitor::ItemType::File,
                                                                                                 TableModelFileMonitor::ProgressStatus::ApplyingAutoAction);

        for(const QString &item : filePathList)
        {
            bool isSaved = LambdaFactoryTabFileMonitor::applyActionForFile()(dbConnectionName(), item);

            if(isSaved)
            {
                LambdaFactoryTabFileMonitor::updateProgressOfRowInModelDb()(dbConnectionName(),
                                                                                  item,
                                                                                  TableModelFileMonitor::ProgressStatus::Completed);
                LambdaFactoryTabFileMonitor::deleteRowFromModelDb()(dbConnectionName(), item);
            }
            else
                LambdaFactoryTabFileMonitor::updateProgressOfRowInModelDb()(dbConnectionName(),
                                                                                  item,
                                                                                  TableModelFileMonitor::ProgressStatus::ErrorOccured);
        }
    });

    categorizationWatcher->setFuture(categorizationFuture);
    savingWatcher->setFuture(savingFuture);
}

void TabFileMonitor::slotOnAsyncTaskCompleted()
{
    for(QFutureWatcher<void> *watcher : qAsConst(resultSet))
    {
        if(watcher->isFinished())
        {
            refreshTableViewFileMonitor();
            watcher->deleteLater();
            resultSet.remove(watcher);
        }
    }
}

void TabFileMonitor::refreshTableViewFileMonitor()
{
    TableModelFileMonitor *tableModel = (TableModelFileMonitor *) ui->tableViewFileMonitor->model();

    if(tableModel == nullptr)
    {
        tableModel = new TableModelFileMonitor(db, ui->tableViewFileMonitor);
        ui->tableViewFileMonitor->setModel(tableModel);
    }

    tableModel->runSelectQuery();

    ui->tableViewFileMonitor->horizontalHeader()->setMinimumSectionSize(140);
    ui->tableViewFileMonitor->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);

    ui->tableViewFileMonitor->hideColumn(TableModelFileMonitor::ColumnIndex::Path);
    ui->tableViewFileMonitor->hideColumn(TableModelFileMonitor::ColumnIndex::AutoSyncStatus);

    if(!tableModel->isRowWithOldNameExist()) // If all columns have NULL value for old_name column.
        ui->tableViewFileMonitor->hideColumn(TableModelFileMonitor::ColumnIndex::OldName);
    else
        ui->tableViewFileMonitor->showColumn(TableModelFileMonitor::ColumnIndex::OldName);

    ui->tableViewFileMonitor->horizontalHeader()->setSectionResizeMode(TableModelFileMonitor::ColumnIndex::Name,
                                                                       QHeaderView::ResizeMode::Interactive);

    ui->tableViewFileMonitor->horizontalHeader()->setSectionResizeMode(TableModelFileMonitor::ColumnIndex::ParentDir,
                                                                       QHeaderView::ResizeMode::Interactive);

    ui->tableViewFileMonitor->horizontalHeader()->setSectionResizeMode(TableModelFileMonitor::ColumnIndex::Path,
                                                                       QHeaderView::ResizeMode::Interactive);

    ui->tableViewFileMonitor->setItemDelegateForColumn(TableModelFileMonitor::ColumnIndex::Action, this->comboBoxItemDelegateFileAction);
    ui->tableViewFileMonitor->setItemDelegateForColumn(TableModelFileMonitor::ColumnIndex::Note, this->comboBoxItemDelegateNote);

    for(int rowIndex = 0; rowIndex < tableModel->rowCount(); rowIndex++)
    {
        ui->tableViewFileMonitor->openPersistentEditor(tableModel->index(rowIndex, TableModelFileMonitor::ColumnIndex::Action));
        ui->tableViewFileMonitor->openPersistentEditor(tableModel->index(rowIndex, TableModelFileMonitor::ColumnIndex::Note));
    }

    ui->tableViewFileMonitor->resizeColumnsToContents();
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

    QString queryString = "CREATE TABLE TableItem (";
    queryString += " name TEXT NOT NULL,";
    queryString += " parent_dir TEXT NOT NULL,";
    queryString += " path TEXT NOT NULL UNIQUE GENERATED ALWAYS AS (parent_dir || name) VIRTUAL,";
    queryString += " old_name TEXT,";
    queryString += " type INTEGER NOT NULL CHECK(type >= 0 AND type <= 2),";
    queryString += " status	INTEGER NOT NULL CHECK(status >= 0 AND status <= 6),";
    queryString += " timestamp TEXT NOT NULL,";
    queryString += " auto_sync_status INTEGER NOT NULL DEFAULT 0 CHECK(auto_sync_status == 0 OR auto_sync_status == 1),";
    queryString += " progress INTEGER,";
    queryString += " current_version INTEGER DEFAULT 0,";
    queryString += " action	INTEGER,";
    queryString += " note TEXT,";
    queryString += " PRIMARY KEY(parent_dir, name)";
    queryString += ");";

    QSqlQuery query(db);
    query.prepare(queryString);
    query.exec();
}


void TabFileMonitor::on_buttonAddNote_clicked()
{
    QStringListModel *model = qobject_cast<QStringListModel *>(ui->comboBoxNoteNumber->model());

    if(model == nullptr)
    {
        model = new QStringListModel(ui->comboBoxNoteNumber);
        ui->comboBoxNoteNumber->setModel(model);
    }

    auto stringList = model->stringList();
    auto size = stringList.size();

    QString item;

    if(size == 0)
        item = "1";
    else
        item = QString::number(stringList.last().toInt() + 1);

    stringList.append(item);
    model->setStringList(stringList);

    ui->comboBoxNoteNumber->setEnabled(true);
    ui->buttonDeleteNote->setEnabled(true);
    ui->textEditDescription->setEnabled(true);

    size = stringList.size();
    ui->comboBoxNoteNumber->setCurrentIndex(size - 1);

    noteMap.insert(ui->comboBoxNoteNumber->currentText().toInt(), "");
    ui->textEditDescription->setText("");

    emit signalNoteNumberAdded(stringList);
}


void TabFileMonitor::on_buttonDeleteNote_clicked()
{
    noteMap.remove(ui->comboBoxNoteNumber->currentText().toInt());

    auto currentIndex = ui->comboBoxNoteNumber->currentIndex();
    QStringListModel *model = qobject_cast<QStringListModel *>(ui->comboBoxNoteNumber->model());
    auto stringList = model->stringList();
    auto previousSize = stringList.size();
    auto removedItem = stringList.value(currentIndex);
    stringList.removeAt(currentIndex);
    model->setStringList(stringList);

    auto size = stringList.size();

    if(size == 0)
    {
        ui->buttonDeleteNote->setDisabled(true);
        ui->comboBoxNoteNumber->setDisabled(true);
        ui->textEditDescription->setDisabled(true);
        ui->textEditDescription->clear();
    }

    if(currentIndex == previousSize - 1) // If deleting the last item
        ui->comboBoxNoteNumber->setCurrentIndex(currentIndex - 1);
    else
        ui->comboBoxNoteNumber->setCurrentIndex(currentIndex); // Stay at same index.

    emit signalNoteNumberDeleted(stringList, removedItem);
}


void TabFileMonitor::on_textEditDescription_textChanged()
{
    noteMap.insert(ui->comboBoxNoteNumber->currentText().toInt(),
                   ui->textEditDescription->toPlainText());
}


void TabFileMonitor::on_comboBoxNoteNumber_currentTextChanged(const QString &arg1)
{
    auto key = ui->comboBoxNoteNumber->currentText().toInt();
    ui->textEditDescription->setText(noteMap.value(key));
}

