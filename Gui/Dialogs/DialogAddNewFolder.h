#ifndef DIALOGADDNEWFOLDER_H
#define DIALOGADDNEWFOLDER_H

#include "BaseDialog.h"

#include <QDialog>

namespace Ui {
class DialogAddNewFolder;
}

class DialogAddNewFolder : public QDialog, public BaseDialog
{
    Q_OBJECT

public:
     DialogAddNewFolder(const QString &parentFolderPath, QWidget *parent = nullptr);
    ~DialogAddNewFolder();

private slots:
    void on_pushButton_clicked();

    void on_lineEdit_textChanged(const QString &arg1);

private:
    Ui::DialogAddNewFolder *ui;
    QString parentFolderPath;
    QLabel *labelStatus;
};

#endif // DIALOGADDNEWFOLDER_H
