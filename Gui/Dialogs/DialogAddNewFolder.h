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

public Q_SLOTS:
    void show();

private:
    QString expectingStatusText();
    QString emptyFolderStatusText();
    QString existStatusText(QString folderName);
    QString successStatusText(QString folderName);
    QString errorStatusText(QString folderName);

private:
    Ui::DialogAddNewFolder *ui;
    QString parentFolderPath;
    QLabel *labelStatus;
};

#endif // DIALOGADDNEWFOLDER_H
