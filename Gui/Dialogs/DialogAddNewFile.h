#ifndef DIALOGADDNEWFILE_H
#define DIALOGADDNEWFILE_H

#include <QDialog>

#include "BaseDialog.h"
#include "DataModels/DialogAddNewFile/TableModelNewAddedFiles.h"
#include "ItemDelegates/DialogAddNewFile/ComboBoxItemDelegateAutoSync.h"

namespace Ui {
class DialogAddNewFile;
}

class DialogAddNewFile : public QDialog, public BaseDialog
{
    Q_OBJECT

public:
     DialogAddNewFile(const QString &targetFolder, QWidget *parent = nullptr);
    ~DialogAddNewFile();

private slots:
    void on_buttonSelectNewFile_clicked();

    void on_buttonRemoveFile_clicked();

    void on_clbAddFilesToDb_clicked();

    void on_clbAddNewFiles_clicked();

    void onTaskAddNewFilesFinished(bool isAllRequestSuccessful);
    void refreshTableView();

signals:
    void signalDisableDelegatesOfAutoSyncColumn(bool flag);

private:
    QLabel *labelStatus;

private:
    Ui::DialogAddNewFile *ui;
    TableModelNewAddedFiles *tableModelNewAddedFiles;
    ComboBoxItemDelegateAutoSync *comboBoxDelegateAutoSync;
    QString targetSymbolFolder;
};

#endif // DIALOGADDNEWFILE_H
