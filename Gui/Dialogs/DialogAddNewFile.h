#ifndef DIALOGADDNEWFILE_H
#define DIALOGADDNEWFILE_H

#include <QDialog>

#include "DataModels/DialogAddNewFile/TableModelNewAddedFiles.h"

namespace Ui {
class DialogAddNewFile;
}

class DialogAddNewFile : public QDialog
{
    Q_OBJECT

public:
    explicit DialogAddNewFile(const QString &targetFolder, QWidget *parent = nullptr);
    ~DialogAddNewFile();

private slots:
    void on_buttonSelectNewFile_clicked();

    void on_buttonRemoveFile_clicked();

    void on_commandLinkButton_clicked();

    void onTaskAddNewFilesFinished(bool isAllRequestSuccessful);

private:
    void showStatusNormal(const QString &message);
    void showStatusInfo(const QString &message);
    void showStatusWarning(const QString &message);
    void showStatusError(const QString &message);
    void showStatusSuccess(const QString &message);

private:
    Ui::DialogAddNewFile *ui;
    TableModelNewAddedFiles *tableModelNewAddedFiles;
    QString targetSymbolFolder;
};

#endif // DIALOGADDNEWFILE_H
