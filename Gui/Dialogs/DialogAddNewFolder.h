#ifndef DIALOGADDNEWFOLDER_H
#define DIALOGADDNEWFOLDER_H

#include "BaseDialog.h"

#include "DataModels/DialogAddNewFolder/CustomFileSystemModel.h"
#include "Backend/FileMonitorSubSystem/FileMonitoringManager.h"

#include <QDialog>
#include <QFileSystemModel>

namespace Ui {
class DialogAddNewFolder;
}

class DialogAddNewFolder : public QDialog, public BaseDialog
{
    Q_OBJECT

public:
    typedef struct
    {
        QString userFolderPath;
        QString symbolFolderPath;
        QHash<QString, bool> files;

    } FolderItem;

    explicit DialogAddNewFolder(QWidget *parent = nullptr);
    ~DialogAddNewFolder();

public slots:
    void show(const QString &parentFolderPath, FileMonitoringManager *fmm);

    // QDialog interface
protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void on_buttonSelectFolder_clicked();
    void on_treeView_doubleClicked(const QModelIndex &index);
    void on_buttonAddFilesToDb_clicked();
    void slotOnTaskAddNewFoldersFinished(bool isAllRequestSuccessful);
    void refreshTreeView();

    void on_buttonClearResults_clicked();

private:
    static qint64 getFolderSize(const QString &pathToFolder);
    QMap<QString, FolderItem> createBufferWithFolderOnly();
    void addFilesToBuffer(QMap<QString, FolderItem> &buffer);
    QString generateSymbolFolderPathFrom(const QString &userFolderPath,
                                         const QString &parentUserFolderPath,
                                         const QString &parentSymbolFolderPath);

    static QString statusTextWaitingForFolder();
    static QString statusTextContentReadyToAdd();
    static QString statusTextEmptyFolder();
    static QString statusTextAdding();
    static QString statusTextNoFreeSpace(QString folderName);
    static QString statusTextFolderExist(QString folderName);
    static QString statusTextSuccess(QString folderName);
    static QString statusTextError(QString folderName);

private:
    Ui::DialogAddNewFolder *ui;
    CustomFileSystemModel *model;
    FileMonitoringManager *fmm;
    QString parentFolderPath;

};

#endif // DIALOGADDNEWFOLDER_H
