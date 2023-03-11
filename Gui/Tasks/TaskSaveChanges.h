#ifndef TASKSAVECHANGES_H
#define TASKSAVECHANGES_H

#include <QSet>
#include <QThread>

#include "DataModels/TabFileMonitor/TreeItem.h"

class TaskSaveChanges : public QThread
{
    Q_OBJECT
public:
    explicit TaskSaveChanges(const QMap<QString, TreeModelFileMonitor::TreeItem *> folderItemMap,
                             const QMap<QString, TreeModelFileMonitor::TreeItem *> fileItemMap,
                             QObject *parent = nullptr);
    ~TaskSaveChanges();

    int getTotalItemCount() const;

signals:
    void folderRestored(const QString &pathToFolder);
    void itemBeingProcessed(int itemNumber);

    // QThread interface
protected:
    void run();

private:
    void saveFolderChanges();
    void saveFileChanges();

    QMapIterator<QString, TreeModelFileMonitor::TreeItem *> folderItemIterator;
    QMapIterator<QString, TreeModelFileMonitor::TreeItem *> fileItemIterator;
    int totalItemCount;
    int currentItemNumber;
};

#endif // TASKSAVECHANGES_H
