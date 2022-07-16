#ifndef TABFILEMONITOR_H
#define TABFILEMONITOR_H

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

public slots:
    void slotOnPredictionTargetNotFound(const QString &pathToFile);
    void slotOnUnPredictedFolderDetected(const QString &pathToFolder);
    void slotOnNewFolderAdded(const QString &pathToFolder);
    void slotOnFolderDeleted(const QString &pathToFolder);
    void slotOnFolderMoved(const QString &pathToFolder);
    void slotOnUnPredictedFileDetected(const QString &pathToFile);
    void slotOnNewFileAdded(const QString &pathToFile);
    void slotOnFileDeleted(const QString &pathToFile);
    void slotOnFileMoved(const QString &pathToFile);
    void slotOnFileModified(const QString &pathToFile);
    void slotOnFileMovedAndModified(const QString &pathToFile);

private:
    void addRowToTableViewFileMonitor(const TableModelFileMonitor::TableItem &item);

private:
    Ui::TabFileMonitor *ui;
    TableModelFileMonitor *tableModelFileMonitor;
    ComboBoxItemDelegateNote *comboBoxItemDelegateNote;
    ComboBoxItemDelegateFileAction *comboBoxItemDelegateFileAction;

};

#endif // TABFILEMONITOR_H
