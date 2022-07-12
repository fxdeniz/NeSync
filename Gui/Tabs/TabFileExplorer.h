#ifndef TABFILEEXPLORER_H
#define TABFILEEXPLORER_H

#include <QWidget>
#include <QMenu>

#include "FileStorageSubSystem/InMemoryDataTypes/FolderMetaData.h"
#include "DataModels/TabFileExplorer/TableModelFileExplorer.h"
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

signals:
    void signalToRouter_ShowRelatedFiles();
    void signalToRouter_ShowDialogTableItemEditor();
    void requestDirContent(const QString &directory);

private slots:
    void on_contextActionListFileExplorer_ShowRelatedFiles_triggered();
    void on_contextActionTableFileExplorer_Edit_triggered();
    void onDirContentFetched(FolderMetaData data);

private:
    void showContextMenuTableView(const QPoint &argPos);
    void showContextMenuListView(const QPoint &argPos);
    void buildContextMenuTableFileExplorer();
    void buildContextMenuListFileExplorer();
    void fillFileExplorerWithRootFolderContents();
    void createNavigationTask();

    QString navigationTaskControllerThreadName() const;

private:
    Ui::TabFileExplorer *ui;
    QMenu *contextMenuTableFileExplorer;
    QMenu *contextMenuListFileExplorer;
    TableModelFileExplorer *tableModelFileExplorer;
    ListModelFileExplorer *listModelFileExplorer;
    QThread *navigationTaskControllerThread;
};

#endif // TABFILEEXPLORER_H
