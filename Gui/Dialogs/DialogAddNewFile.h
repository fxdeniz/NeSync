#ifndef DIALOGADDNEWFILE_H
#define DIALOGADDNEWFILE_H

#include <QDialog>

#include "DataModels/DialogAddNewFile/TableModelNewAddedFiles.h"
#include "Backend/FileStorageSubSystem/FileStorageManager.h"

namespace Ui {
class DialogAddNewFile;
}

class DialogAddNewFile : public QDialog
{
    Q_OBJECT

public:
    explicit DialogAddNewFile(FileStorageManager *fsm, const QString &targetFolder, QWidget *parent = nullptr);
    ~DialogAddNewFile();

private slots:
    void on_buttonSelectNewFile_clicked();

    void on_buttonRemoveFile_clicked();

    void on_commandLinkButton_clicked();

private:
    void showStatusNormal(const QString &message);
    void showStatusInfo(const QString &message);
    void showStatusWarning(const QString &message);
    void showStatusError(const QString &message);
    bool postToFSM(const QString &pathToFile);

    QScopedPointer<FileStorageManager> createFSM() const;

private:
    Ui::DialogAddNewFile *ui;
    TableModelNewAddedFiles *tableModelNewAddedFiles;
    FileStorageManager *fileStorageManager;
    QString targetSymbolFolder;
};

#endif // DIALOGADDNEWFILE_H
