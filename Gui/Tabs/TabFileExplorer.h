#ifndef TABFILEEXPLORER_H
#define TABFILEEXPLORER_H

#include <QWidget>
#include <QMenu>

#include "FileStorageSubSystem/RequestResults/FolderRequestResult.h"
#include "DataModels/TabFileExplorer/ListModelFileExplorer.h"

namespace Ui {
class TabFileExplorer;
}

class TabFileExplorer : public QWidget
{
    Q_OBJECT

public:
    explicit TabFileExplorer(QWidget *parent = nullptr);
    virtual ~TabFileExplorer();

    QString currentDir() const;

signals:
    void signalToRouter_ShowRelatedFiles();
    void signalToRouter_ShowDialogTableItemEditor();
    void signalRequestDirContent(const QString &directory);

private slots:
    void on_contextActionListFileExplorer_ShowRelatedFiles_triggered();
    void on_contextActionTableFileExplorer_Edit_triggered();
    void slotOnDirContentFetched(FolderRequestResult data);

private:
    void showContextMenuTableView(const QPoint &argPos);
    void showContextMenuListView(const QPoint &argPos);
    void buildContextMenuTableFileExplorer();
    void buildContextMenuListFileExplorer();
    void fillFileExplorerWithRootFolderContents();
    void createNavigationTask();

    QString navigationTaskThreadName() const;

private:
    Ui::TabFileExplorer *ui;
    QMenu *contextMenuTableFileExplorer;
    QMenu *contextMenuListFileExplorer;
    ListModelFileExplorer *listModelFileExplorer;
    QThread *navigationTaskThread;

};

#endif // TABFILEEXPLORER_H
