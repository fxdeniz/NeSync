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
     DialogAddNewFile(QWidget *parent = nullptr);
    ~DialogAddNewFile();

public slots:
    void show(const QString &targetFolder);

signals:
     void signalDisableDelegatesOfAutoSyncColumn(bool flag);

     // QDialog interface
protected:
     void closeEvent(QCloseEvent *event);

private slots:
    void on_buttonSelectNewFile_clicked();
    void on_buttonRemoveFile_clicked();
    void on_clbAddFilesToDb_clicked();
    void on_clbAddNewFiles_clicked();

    void onTaskAddNewFilesFinished(bool isAllRequestSuccessful);
    void refreshTableView();

private:
    QLabel *labelStatus;

private:
    Ui::DialogAddNewFile *ui;
    TableModelNewAddedFiles *tableModelNewAddedFiles;
    ComboBoxItemDelegateAutoSync *comboBoxDelegateAutoSync;
    QString targetSymbolFolder;
};

#endif // DIALOGADDNEWFILE_H
