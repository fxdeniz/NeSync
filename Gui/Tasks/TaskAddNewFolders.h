#ifndef TASKADDNEWFOLDERS_H
#define TASKADDNEWFOLDERS_H

#include <QThread>
#include <QObject>
#include <QHash>

class TaskAddNewFolders : public QThread
{
    Q_OBJECT
public:
    explicit TaskAddNewFolders(const QHash<QString, QString> &userDirToSymbolDirMapping,
                               QObject *parent = nullptr);
    ~TaskAddNewFolders();

    void addFile(const QString &userFilePath, bool isAutoSyncEnabled);
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
    void createSymbolDirs();

    QHashIterator<QString, QString> *hashIterator;
    QHash<QString, bool> files;

//    typedef struct
//    {
//        bool isAutoSyncEnabled;
//        QString symbolFolderPath;
//    } FileItem;

//    QHash<QString, FileItem> files;
};

#endif // TASKADDNEWFOLDERS_H
