#ifndef V2_DIALOGADDNEWFOLDER_H
#define V2_DIALOGADDNEWFOLDER_H

#include <QDialog>
#include <QFileSystemModel>

#include "BaseDialog.h"
#include "DataModels/DialogAddNewFolder/CustomFileSystemModel.h"

namespace Ui {
class V2_DialogAddNewFolder;
}

class V2_DialogAddNewFolder : public QDialog, public BaseDialog
{
    Q_OBJECT

public:
    explicit V2_DialogAddNewFolder(QWidget *parent = nullptr);
    ~V2_DialogAddNewFolder();

public slots:
    void show(const QString &_parentFolderPath);

private slots:
    void on_buttonSelectFolder_clicked();
    void on_treeView_doubleClicked(const QModelIndex &index);
    void on_buttonAddFilesToDb_clicked();
    void on_buttonTest_V2_RowFolderRecord_clicked();
    void slotEnableButtonAddFilesToDb(const QString &dummy);

private:
    static QString statusTextWaitingForFolder();
    static QString statusTextContentReadyToAdd(uint folderCount, uint fileCount);
    static QString statusTextEmptyFolder();
    static QString statusTextExist(QString folderName);
    static QString statusTextSuccess(QString folderName);
    static QString statusTextError(QString folderName);

private:
    Ui::V2_DialogAddNewFolder *ui;
    CustomFileSystemModel *model;
    QString parentFolderPath;
};

#endif // V2_DIALOGADDNEWFOLDER_H
