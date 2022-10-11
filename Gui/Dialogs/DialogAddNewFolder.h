#ifndef DIALOGADDNEWFOLDER_H
#define DIALOGADDNEWFOLDER_H

#include <QDialog>
#include <QFileSystemModel>

#include "BaseDialog.h"
#include "DataModels/DialogAddNewFolder/CustomFileSystemModel.h"

namespace Ui {
class DialogAddNewFolder;
}

class DialogAddNewFolder : public QDialog, public BaseDialog
{
    Q_OBJECT

public:
    typedef struct
    {
        QString userDir;
        QString symbolDir;
        QHash<QString, bool> files;

    } FolderItem;

    explicit DialogAddNewFolder(QWidget *parent = nullptr);
    ~DialogAddNewFolder();

public slots:
    void show(const QString &_parentFolderPath);

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
    QString generateSymbolDirFrom(const QString &userDir, const QString &parentUserDir, const QString &parentSymbolDir);

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
    QString parentFolderPath;

};

#endif // DIALOGADDNEWFOLDER_H
