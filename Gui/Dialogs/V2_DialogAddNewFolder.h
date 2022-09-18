#ifndef V2_DIALOGADDNEWFOLDER_H
#define V2_DIALOGADDNEWFOLDER_H

#include <QDialog>

namespace Ui {
class V2_DialogAddNewFolder;
}

class V2_DialogAddNewFolder : public QDialog
{
    Q_OBJECT

public:
    explicit V2_DialogAddNewFolder(QWidget *parent = nullptr);
    ~V2_DialogAddNewFolder();

private:
    Ui::V2_DialogAddNewFolder *ui;
};

#endif // V2_DIALOGADDNEWFOLDER_H
