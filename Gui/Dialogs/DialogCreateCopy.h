#ifndef DIALOGCREATECOPY_H
#define DIALOGCREATECOPY_H

#include "BaseDialog.h"

#include <QDialog>

namespace Ui {
class DialogCreateCopy;
}

class DialogCreateCopy : public QDialog, public BaseDialog
{
    Q_OBJECT

public:
    explicit DialogCreateCopy(QWidget *parent = nullptr);
    ~DialogCreateCopy();

    void show(const QString &fileSymbolPath, qlonglong versionNumber);

private slots:
    void on_buttonSelectLocation_clicked();
    void on_buttonCreateCopy_clicked();

private:
    Ui::DialogCreateCopy *ui;
    QString currentFileSymbolPath;

};

#endif // DIALOGCREATECOPY_H
