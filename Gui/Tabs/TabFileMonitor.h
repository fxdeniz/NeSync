#ifndef TABFILEMONITOR_H
#define TABFILEMONITOR_H

#include <QStringListModel>
#include <QSqlDatabase>
#include <QFuture>
#include <QWidget>

#include "ItemDelegates/TabFileMonitor/ComboBoxItemDelegateFileAction.h"
#include "ItemDelegates/TabFileMonitor/NoteColumnDelegate/ComboBoxItemDelegateNote.h"

namespace Ui {
class TabFileMonitor;
}

class TabFileMonitor : public IComboBoxNoteNotifier
{
    Q_OBJECT

public:
    explicit TabFileMonitor(QWidget *parent = nullptr);
    ~TabFileMonitor();

    static QString dbConnectionName();
    static QString dbFileName();

public slots:
    void slotOnActionSaveAllTriggered();
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
    void slotOnAsyncTaskCompleted();

    void on_buttonAddNote_clicked();
    void on_buttonDeleteNote_clicked();
    void on_textEditDescription_textChanged();
    void on_comboBoxNoteNumber_currentTextChanged(const QString &arg1);

private:
    void refreshTableViewFileMonitor();
    void createDb();

private:
    Ui::TabFileMonitor *ui;
    QSqlDatabase db;
    QHash<int, QString> noteMap;
    ComboBoxItemDelegateNote *comboBoxItemDelegateNote;
    ComboBoxItemDelegateFileAction *comboBoxItemDelegateFileAction;
    QSet<QFutureWatcher<void> *> resultSet;
};

#endif // TABFILEMONITOR_H
