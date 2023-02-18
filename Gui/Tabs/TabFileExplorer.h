#ifndef TABFILEEXPLORER_H
#define TABFILEEXPLORER_H

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
    void slotRefreshFileExplorer();

private slots:
    void on_tableViewFileExplorer_clicked(const QModelIndex &index);
    void on_tableViewFileExplorer_doubleClicked(const QModelIndex &index);
    void on_listView_clicked(const QModelIndex &index);
    void on_buttonBack_clicked();
    void on_buttonForward_clicked();

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
    Ui::TabFileExplorer *ui;
    QMenu *contextMenuTableFileExplorer;
    QMenu *contextMenuListFileExplorer;
    QStringList navigationHistoryIndices;

};

#endif // TABFILEEXPLORER_H
