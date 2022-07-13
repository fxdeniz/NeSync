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
     DialogAddNewFolder(QWidget *parent = nullptr);
    ~DialogAddNewFolder();

 public slots:
     void show(const QString &_parentFolderPath);

private slots:
    void on_pushButton_clicked();
    void on_lineEdit_textChanged(const QString &arg1);

    // QDialog interface
protected:
    void closeEvent(QCloseEvent *event);

private:
    QString expectingStatusText();
    QString emptyFolderStatusText();
    QString existStatusText(QString folderName);
    QString successStatusText(QString folderName);
    QString errorStatusText(QString folderName);

private:
    Ui::DialogAddNewFolder *ui;
    QString parentFolderPath;

};

#endif // DIALOGADDNEWFOLDER_H
