#ifndef TASKSAVECHANGES_H
#define TASKSAVECHANGES_H

#include <QSet>
#include <QThread>

#include "DataModels/TabFileMonitor/TreeItem.h"

class TaskSaveChanges : public QThread
{
    Q_OBJECT
public:
    explicit TaskSaveChanges(const QHash<QString, TreeItem *> folderItemMap,
                             const QHash<QString, TreeItem *> fileItemMap,
                             QObject *parent = nullptr);
    ~TaskSaveChanges();

signals:
    void folderRestored(const QString &pathToFolder);

    // QThread interface
protected:
    void run();

private:
    void saveFolderChanges();
    void saveFileChanges();

    QHashIterator<QString, TreeItem *> folderItemIterator;
    QHashIterator<QString, TreeItem *> fileItemIterator;
};

#endif // TASKSAVECHANGES_H
