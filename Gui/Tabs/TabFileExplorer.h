#ifndef TABFILEEXPLORER_H
#define TABFILEEXPLORER_H

#include "Dialogs/DialogEditVersion.h"
#include "Dialogs/DialogCreateCopy.h"
#include "Dialogs/DialogExport.h"

#include <QMenu>
#include <QWidget>
#include <QJsonObject>

namespace Ui {
class TabFileExplorer;
}

class TabFileExplorer : public QWidget
{
    Q_OBJECT

public:
    explicit TabFileExplorer(QWidget *parent = nullptr);
    virtual ~TabFileExplorer();

    QString currentSymbolFolderPath() const;

public slots:
    void refreshFileExplorer();

signals:
    void signalStopFileMonitor();
    void signalStartFileMonitor();
    void signalStopMonitoringItem(const QString &userPathToFileOrFolder);
    void signalStartMonitoringItem(const QString &userPathToFileOrFolder);
    void signalRefreshFileMonitor();

private slots:
    void on_tableView_clicked(const QModelIndex &index);
    void on_tableView_doubleClicked(const QModelIndex &index);
    void on_listView_clicked(const QModelIndex &index);
    void on_buttonBack_clicked();
    void on_buttonForward_clicked();

    void on_contextActionTableView_Export_triggered();
    void on_contextActionTableView_Delete_triggered();
    void on_contextActionTableView_Freeze_triggered();
    void on_contextActionListView_EditVersion_triggered();
    void on_contextActionListView_DeleteVersion_triggered();
    void on_contextActionListView_CreateCopy_triggered();
    void on_contextActionListView_SetAsCurrentVersion_triggered();

private:
    void clearDescriptionDetails();
    void showContextMenuTableView(const QPoint &argPos);
    void showContextMenuListView(const QPoint &argPos);
    void buildContextMenuTableFileExplorer();
    void buildContextMenuListFileExplorer();

    void createNavigationHistoryIndex(const QString &path);
    void displayFolderInTableViewFileExplorer(const QString &symbolFolderPath);

    QString fileSizeToString(qulonglong fileSize) const;

private:
    void executeFreezingOrThawingOfFolder(const QString &name, const QString &symbolPath, const QString &userPath, bool isFrozen);
    void executeFreezingOrThawingOfFile(const QString &name, const QString &symbolPath, const QString &userPath, bool isFrozen);
    void thawFolderTree(const QString folderName, const QString &parentSymbolFolderPath, const QString &targetUserPath);

    Ui::TabFileExplorer *ui;
    DialogEditVersion *dialogEditVersion;
    DialogCreateCopy *dialogCreateCopy;
    DialogExport *dialogExport;
    QMenu *contextMenuTableFileExplorer;
    QMenu *contextMenuListFileExplorer;
    QStringList navigationHistoryIndices;

};

#endif // TABFILEEXPLORER_H
