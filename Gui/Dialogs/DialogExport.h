#ifndef DIALOGEXPORT_H
#define DIALOGEXPORT_H

#include "BaseDialog.h"

#include <QDialog>

namespace Ui {
class DialogExport;
}

class DialogExport : public QDialog, BaseDialog
{
    Q_OBJECT

public:
    explicit DialogExport(QWidget *parent = nullptr);
    ~DialogExport();

    void show(QList<QString> itemList);

signals:
    void signalZipCreationStarted(int minimum, int maximum);
    void signalZipProgressUpdated(int value);
    void signalAddingFileToZip(const QString &symbolFilePath);
    void signalZippingFinished(bool finshedSuccessfully);

private slots:
    void on_buttonSelectLocation_clicked();
    void on_buttonExport_clicked();

private:
    static QString statusTextSelectLocation();
    static QString statusTextZipFileReadyToCreate();
    static QString statusTextZippingInProgress();
    static QString statusTextZippingFinishedWithoutError();
    static QString statusTextZippingFinishedWithError();

    Ui::DialogExport *ui;
    QList<QString> itemList;
};

#endif // DIALOGEXPORT_H
