#ifndef V2_DIALOGADDNEWFOLDER_H
#define V2_DIALOGADDNEWFOLDER_H

#include <QDialog>
#include <QFileSystemModel>

#include "BaseDialog.h"

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

private:
    Ui::V2_DialogAddNewFolder *ui;
    QFileSystemModel *model;
};

#endif // V2_DIALOGADDNEWFOLDER_H
