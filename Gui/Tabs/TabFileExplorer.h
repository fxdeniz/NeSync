#ifndef TABFILEEXPLORER_H
#define TABFILEEXPLORER_H

#include <QMenu>
#include <QWidget>
#include <QJsonObject>

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

public slots:
    void slotRefreshFileExplorer();

signals:
    void signalToRouter_ShowRelatedFiles();
    void signalToRouter_ShowDialogTableItemEditor();
    void signalRequestDirContent(const QString &directory);

private slots:
    void slotOnDirContentFetched(QJsonObject result);

    void on_contextActionListFileExplorer_ShowRelatedFiles_triggered();
    void on_contextActionTableFileExplorer_Edit_triggered();
    void on_tableViewFileExplorer_doubleClicked(const QModelIndex &index);
    void on_buttonBack_clicked();
    void on_buttonForward_clicked();

private:
    void showContextMenuTableView(const QPoint &argPos);
    void showContextMenuListView(const QPoint &argPos);
    void buildContextMenuTableFileExplorer();
    void buildContextMenuListFileExplorer();
    void fillTableFileExplorerWith(const QString &symbolDirPath);
    void createNavigationTask();

    void createNavigationHistoryIndex(const QString &path);
    QString navigationTaskThreadName() const;
    void displayInTableViewFileExplorer(QJsonObject result);

private:
    Ui::TabFileExplorer *ui;
    QMenu *contextMenuTableFileExplorer;
    QMenu *contextMenuListFileExplorer;
    ListModelFileExplorer *listModelFileExplorer;
    QThread *navigationTaskThread;
    QStringList navigationHistoryIndices;

};

#endif // TABFILEEXPLORER_H
