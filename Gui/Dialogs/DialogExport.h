#ifndef DIALOGEXPORT_H
#define DIALOGEXPORT_H

#include <QDialog>

namespace Ui {
class DialogExport;
}

class DialogExport : public QDialog
{
    Q_OBJECT

public:
    explicit DialogExport(QWidget *parent = nullptr);
    ~DialogExport();

    void show();

private slots:
    void on_buttonSelectLocation_clicked();

private:
    Ui::DialogExport *ui;
};

#endif // DIALOGEXPORT_H
