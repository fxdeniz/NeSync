#ifndef TASKADDNEWFOLDERS_H
#define TASKADDNEWFOLDERS_H

#include "Dialogs/V2_DialogAddNewFolder.h"

#include <QThread>
#include <QObject>
#include <QHash>

class TaskAddNewFolders : public QThread
{
    Q_OBJECT
public:
    explicit TaskAddNewFolders(const QList<V2_DialogAddNewFolder::FolderItem> &_list,
                               QObject *parent = nullptr);
    ~TaskAddNewFolders();
    int fileCount() const;

    // QThread interface
protected:
    void run() override;

signals:
    void signalGenericFileEvent();
    void signalFileBeingProcessed(const QString &pathToFile);
    void signalFileAddedSuccessfully(const QString &pathToFile);
    void signalFileAddingFailed(const QString &pathToFile);
    void signalFileProcessed(int fileNumber);
    void finished(bool isAllRequestsSuccessful); // Overloaded QThread::finished()

private:
    QList<V2_DialogAddNewFolder::FolderItem> list;
};

#endif // TASKADDNEWFOLDERS_H
