#ifndef DIALOGIMPORT_H
#define DIALOGIMPORT_H

#include "BaseDialog.h"
#include "DataModels/DialogImport/ItemDelegateAction.h"

#include <QDialog>
#include <QFutureWatcher>

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

signals:
    void signalProgressUpdate(int value);
    void signalFileImportStartedForActiveFile(const QString &userFilePath);
    void signalFileImportStarted(const QString &symbolFilePath);
    void signalFileImported(const QString &symbolFilePath);
    void signalFileImportFailed(const QString &symbolFilePath);

private slots:
    void on_buttonSelectFile_clicked();
    void on_buttonImport_clicked();
    void on_buttonClearResults_clicked();

private:
    static QString statusTextWaitingForZipFile();
    static QString statusTextCanNotOpenFile(const QString &fileNameArg);
    static QString statusTextImportJsonFileMissing();
    static QString statusTextImportJsonFileCorrupt();
    static QString statusTextZipFileReadyToImport();
    static QString statusTextFilesBeingImported();
    static QString statusTextFileImportFinishedWithoutError();
    static QString statusTextFileImportFinishedWithError();

    Ui::DialogImport *ui;
    TreeModelDialogImport::ItemDelegateAction *itemDelegateAction;
    QFutureWatcher<void> futureWatcher;
    bool allFilesImportedSuccessfully;
};

#endif // DIALOGIMPORT_H
