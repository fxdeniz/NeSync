#ifndef TABFILEMONITOR_H
#define TABFILEMONITOR_H

#include <QSqlDatabase>
#include <QFuture>
#include <QWidget>

#include "ItemDelegates/TabFileMonitor/ComboBoxItemDelegateFileAction.h"
#include "ItemDelegates/TabFileMonitor/ComboBoxItemDelegateNote.h"
#include "DataModels/TabFileMonitor/TableModelFileMonitor.h"

namespace Ui {
class TabFileMonitor;
}

class TabFileMonitor : public QWidget
{
    Q_OBJECT

public:
    explicit TabFileMonitor(QWidget *parent = nullptr);
    ~TabFileMonitor();

    static QString dbConnectionName();
    static QString dbFileName();

public slots:
    void slotOnPredictionTargetNotFound(const QString &pathToFile);
    void slotOnUnPredictedFolderDetected(const QString &pathToFolder);
    void slotOnNewFolderAdded(const QString &pathToFolder);
    void slotOnFolderDeleted(const QString &pathToFolder);
    void slotOnFolderMoved(const QString &pathToFolder, const QString &oldFolderName);
    void slotOnUnPredictedFileDetected(const QString &pathToFile);
    void slotOnNewFileAdded(const QString &pathToFile);
    void slotOnFileDeleted(const QString &pathToFile);
    void slotOnFileMoved(const QString &pathToFile, const QString &oldFileName);
    void slotOnFileModified(const QString &pathToFile);
    void slotOnFileMovedAndModified(const QString &pathToFile, const QString &oldFileName);

signals:
    void signalTableItemReady(TableModelFileMonitor::TableItem item);

private slots:
    void slotRefreshTableViewFileMonitor();
    void addRowToTableViewFileMonitor(const TableModelFileMonitor::TableItem &item);

private:
    void createDb();

private:
    Ui::TabFileMonitor *ui;
    QSqlDatabase db;
    ComboBoxItemDelegateNote *comboBoxItemDelegateNote;
    ComboBoxItemDelegateFileAction *comboBoxItemDelegateFileAction;
    QSet<QFutureWatcher<TableModelFileMonitor::TableItem> *> resultSet;

};

#endif // TABFILEMONITOR_H
