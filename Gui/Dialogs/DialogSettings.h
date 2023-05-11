#ifndef DIALOGSETTINGS_H
#define DIALOGSETTINGS_H

#include "BaseDialog.h"

#include <QDialog>

namespace Ui {
class DialogSettings;
}

class DialogSettings : public QDialog, public BaseDialog
{
    Q_OBJECT

public:
    explicit DialogSettings(QWidget *parent = nullptr);
    ~DialogSettings();

    void show();

private slots:
    void on_buttonSelectStorageFolder_clicked();

    void on_buttonSave_clicked();

private:
    Ui::DialogSettings *ui;
};

#endif // DIALOGSETTINGS_H
