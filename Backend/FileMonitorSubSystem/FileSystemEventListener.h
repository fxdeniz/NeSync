#ifndef FILESYSTEMEVENTLISTENER
#define FILESYSTEMEVENTLISTENER

#include <QObject>

#include "Backend/FileMonitorSubSystem/efsw/efsw.hpp"

class FileSystemEventListener : public QObject, public efsw::FileWatchListener
{
    Q_OBJECT
public:
    explicit FileSystemEventListener(QObject *parent = nullptr);

signals:
    void signalAddEventDetected(const QString &fileName, const QString &dir);
    void signalDeleteEventDetected(const QString &fileName, const QString &dir);
    void signalModificationEventDetected(const QString &fileName, const QString &dir);
    void signalMoveEventDetected(const QString &newFileName, const QString &oldFileName, const QString &dir);

    // FileWatchListener interface
public:
    void handleFileAction(efsw::WatchID watchid,
                          const std::string &dir,
                          const std::string &filename,
                          efsw::Action action,
                          std::string oldFilename) override;
};

#endif // FILESYSTEMEVENTLISTENER
