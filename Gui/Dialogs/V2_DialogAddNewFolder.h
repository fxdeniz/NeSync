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

private slots:
    void on_buttonSelectFolder_clicked();

    void on_treeView_doubleClicked(const QModelIndex &index);

private:
    Ui::V2_DialogAddNewFolder *ui;
    CustomFileSystemModel *model;
};

#endif // V2_DIALOGADDNEWFOLDER_H
