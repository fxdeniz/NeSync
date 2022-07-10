#ifndef DIALOGADDNEWFOLDER_H
#define DIALOGADDNEWFOLDER_H

#include <QDialog>

namespace Ui {
class DialogAddNewFolder;
}

class DialogAddNewFolder : public QDialog
{
    Q_OBJECT

public:
    explicit DialogAddNewFolder(QWidget *parent = nullptr);
    ~DialogAddNewFolder();

private:
    Ui::DialogAddNewFolder *ui;
};

#endif // DIALOGADDNEWFOLDER_H
