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

private:
    void createSymbolDirs();

    QHashIterator<QString, QString> *hashIterator;

    typedef struct
    {
        bool isAutoSyncEnabled;
        QString symbolFolderPath;
    } FileItem;

    QHash<QString, FileItem> files;
};

#endif // TASKADDNEWFOLDERS_H
