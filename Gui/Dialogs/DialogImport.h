#ifndef DIALOGIMPORT_H
#define DIALOGIMPORT_H

#include "BaseDialog.h"
#include "DataModels/DialogImport/ItemDelegateAction.h"

#include <QDialog>

namespace Ui {
class DialogImport;
}

class DialogImport : public QDialog, public BaseDialog
{
    Q_OBJECT

public:
    explicit DialogImport(QWidget *parent = nullptr);
    ~DialogImport();

    void show();

private slots:
    void on_buttonSelectFile_clicked();

private:
    static QString statusTextWaitingForFile();
    static QString statusTextCanNotOpenFile(const QString &fileNameArg);
    static QString statusTextImportJsonFileMissing();
    static QString statusTextImportJsonFileCorrupt();
    static QString statusTextFileReadyToImport();

    Ui::DialogImport *ui;
    TreeModelDialogImport::ItemDelegateAction *itemDelegateAction;
};

#endif // DIALOGIMPORT_H
