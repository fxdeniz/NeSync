#include "FileSystemEventListener.h"

#include <iostream>

FileSystemEventListener::FileSystemEventListener(QObject *parent)
    : QObject{parent}
{

}

void FileSystemEventListener::handleFileAction(efsw::WatchID watchid,
                                               const std::string &dir,
                                               const std::string &filename,
                                               efsw::Action action,
                                               std::string oldFilename)
{
    auto _dir = QString::fromStdString(dir);
    auto _fileName = QString::fromStdString(filename);
    auto _oldFileName = QString::fromStdString(oldFilename);

    switch( action )
    {
    case efsw::Actions::Add:
        std::cout << "DIR (" << dir << ") FILE (" << filename << ") has event Added" << std::endl;
        emit signalAddEventDetected(_fileName, _dir);
        break;

    case efsw::Actions::Delete:
        std::cout << "DIR (" << dir << ") FILE (" << filename << ") has event Delete" << std::endl;
        emit signalDeleteEventDetected(_fileName, _dir);
        break;

    case efsw::Actions::Modified:
        std::cout << "DIR (" << dir << ") FILE (" << filename << ") has event Modified" << std::endl;
        emit signalModificationEventDetected(_fileName, _dir);
        break;

    case efsw::Actions::Moved:
        std::cout << "DIR (" << dir << ") FILE (" << filename << ") has event Moved from (" << oldFilename << ")" << std::endl;
        emit signalMoveEventDetected(_fileName, _oldFileName, _dir);
        break;

    default:
        std::cout << "Should never happen!" << std::endl;
    }
}
