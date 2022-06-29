#ifndef TABFILEEXPLORER_H
#define TABFILEEXPLORER_H

#include <QWidget>
#include <QMenu>

#include "ListModelFileExplorer.h"
#include "TableModelFileExplorer.h"


namespace Ui {
class TabFileExplorer;
}

class TabFileExplorer : public QWidget
{
    Q_OBJECT

public:
    explicit TabFileExplorer(QWidget *parent = nullptr);
    ~TabFileExplorer();

private slots:
    void on_actionEditTableItem_clicked();


private:
    void showContextMenuTableView(const QPoint &argPos);
    void showContextMenuListView(const QPoint &argPos);
    void buildContextMenuTableFileExplorer();
    void buildContextMenuListFileExplorer();

private:
    Ui::TabFileExplorer *ui;
    QMenu *contextMenuTableFileExplorer;
    QMenu *contextMenuListFileExplorer;
    TableModelFileExplorer *tableModelFileExplorer;
    ListModelFileExplorer *listModelFileExplorer;

};

#endif // TABFILEEXPLORER_H
