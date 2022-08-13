#ifndef TABFILEMONITOR_H
#define TABFILEMONITOR_H

#include <QStringListModel>
#include <QSqlDatabase>
#include <QFuture>
#include <QWidget>

#include "ItemDelegates/TabFileMonitor/ComboBoxItemDelegateFileAction.h"
#include "ItemDelegates/TabFileMonitor/ComboBoxItemDelegateNote.h"

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
    static QString defaultNoNoteText();

    QStringListModel *getListModelNoteNumber() const;

public slots:
    void slotOnPredictionTargetNotFound(const QString &pathToFileOrFolder);
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

private slots:
    void slotOnAsyncCategorizationTaskCompleted();

    void on_buttonAddNote_clicked();
    void on_buttonDeleteNote_clicked();

private:
    void refreshTableViewFileMonitor();
    void createDb();

private:
    Ui::TabFileMonitor *ui;
    QSqlDatabase db;
    ComboBoxItemDelegateNote *comboBoxItemDelegateNote;
    ComboBoxItemDelegateFileAction *comboBoxItemDelegateFileAction;
    QSet<QFutureWatcher<void> *> resultSet;
    QStringListModel *listModelNoteNumber;
};

#endif // TABFILEMONITOR_H
