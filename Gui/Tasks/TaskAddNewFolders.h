#ifndef TASKADDNEWFOLDERS_H
#define TASKADDNEWFOLDERS_H

#include "Dialogs/DialogAddNewFolder.h"

#include <QThread>
#include <QObject>
#include <QHash>

class TaskAddNewFolders : public QThread
{
    Q_OBJECT
public:
    explicit TaskAddNewFolders(QList<DialogAddNewFolder::FolderItem> list,
                               QObject *parent = nullptr);
    ~TaskAddNewFolders();
    int fileCount() const;

    // QThread interface
protected:
    void run() override;

signals:
    void signalGenericFileEvent();
    void signalFolderAdded(const QString &pathToFolder);
    void signalFileBeingProcessed(const QString &pathToFile);
    void signalFileAddedSuccessfully(const QString &pathToFile);
    void signalFileAddingFailed(const QString &pathToFile);
    void signalFileProcessed(int fileNumber);
    void finished(bool isAllRequestsSuccessful); // Overloaded QThread::finished()

private:
    QList<DialogAddNewFolder::FolderItem> list;
};

#endif // TASKADDNEWFOLDERS_H
