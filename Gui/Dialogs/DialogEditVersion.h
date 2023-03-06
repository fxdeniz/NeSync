#ifndef DIALOGEDITVERSION_H
#define DIALOGEDITVERSION_H

#include "BaseDialog.h"

#include <QDialog>

namespace Ui {
class DialogEditVersion;
}

class DialogEditVersion : public QDialog, public BaseDialog
{
    Q_OBJECT

public:
    explicit DialogEditVersion(QWidget *parent = nullptr);
    ~DialogEditVersion();

    void show(const QString &fileSymbolPath, qlonglong versionNumber);

private slots:
    void on_comboBox_currentTextChanged(const QString &arg1);
    void on_buttonRestore_clicked();
    void on_buttonSave_clicked();

private:
    Ui::DialogEditVersion *ui;
    QString currentFileSymbolPath;
};

#endif // DIALOGEDITVERSION_H
