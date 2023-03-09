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

    void show(QList<QString> itemList);

signals:
    void signalZipCreationStarted(int minimum, int maximum);
    void signalZipProgressUpdated(int value);
    void signalZippingFinished();

private slots:
    void on_buttonSelectLocation_clicked();
    void on_buttonExport_clicked();

private:
    Ui::DialogExport *ui;
    QList<QString> itemList;
};

#endif // DIALOGEXPORT_H
