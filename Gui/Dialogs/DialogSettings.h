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

private:
    Ui::DialogSettings *ui;
};

#endif // DIALOGSETTINGS_H
