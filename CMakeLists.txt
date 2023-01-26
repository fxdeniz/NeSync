cmake_minimum_required(VERSION 3.5)

project(BoldBackup VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Sql Concurrent Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Sql Concurrent Widgets)

include_directories(Gui/)
include_directories(Gui/Tabs/)
include_directories(Backend/)
include_directories(Backend/FileStorageSubSystem/ORM/)
include_directories(Backend/FileMonitorSubSystem/)
include_directories(Backend/FileMonitorSubSystem/Backend/FileMonitorSubSystem/efsw/)
include_directories(Backend/FileMonitorSubSystem/Backend/FileMonitorSubSystem/efsw/Test/)
include_directories(Backend/FileMonitorSubSystem/Backend/FileMonitorSubSystem/efsw/platform/)
include_directories(Backend/FileMonitorSubSystem/Backend/FileMonitorSubSystem/efsw/platform/posix/)
include_directories(Backend/FileMonitorSubSystem/Backend/FileMonitorSubSystem/efsw/platform/win/)


set(PROJECT_SOURCES
    Gui/MainWindow.h Gui/MainWindow.cpp Gui/MainWindow.ui

    # BaseDialog
        Gui/Dialogs/BaseDialog.h
        Gui/Dialogs/BaseDialog.cpp
    #

    # DialogAddNewFolder
        Gui/DataModels/DialogAddNewFolder/CustomFileSystemModel.h
        Gui/DataModels/DialogAddNewFolder/CustomFileSystemModel.cpp
        #
        Gui/Dialogs/DialogAddNewFolder.h
        Gui/Dialogs/DialogAddNewFolder.cpp
        Gui/Dialogs/DialogAddNewFolder.ui
    #

    # DialogFileOrDirEditor
        Gui/Dialogs/DialogFileOrDirEditor.h
        Gui/Dialogs/DialogFileOrDirEditor.cpp
        Gui/Dialogs/DialogFileOrDirEditor.ui
    #
	
    # TabFileExplorer
        Gui/Tabs/TabFileExplorer.h
        Gui/Tabs/TabFileExplorer.cpp
        Gui/Tabs/TabFileExplorer.ui
        #
        Gui/DataModels/TabFileExplorer/TableModelFileExplorer.h
        Gui/DataModels/TabFileExplorer/TableModelFileExplorer.cpp
        #
        Gui/DataModels/TabFileExplorer/ListModelFileExplorer.h
        Gui/DataModels/TabFileExplorer/ListModelFileExplorer.cpp
    #

    # TabFileMonitor
        Gui/Tabs/TabFileMonitor.h
        Gui/Tabs/TabFileMonitor.cpp
        Gui/Tabs/TabFileMonitor.ui
    #

    #   DialogDebugFileMonitor
        Gui/Dialogs/DialogDebugFileMonitor.h
        Gui/Dialogs/DialogDebugFileMonitor.cpp
        Gui/Dialogs/DialogDebugFileMonitor.ui
    #

    # TabReleatedFiles
        Gui/Tabs/TabRelatedFiles.h
        Gui/Tabs/TabRelatedFiles.cpp
        Gui/Tabs/TabRelatedFiles.ui
    #

    # Tasks
        # TaskAddNewFiles
        Gui/Tasks/TaskAddNewFiles.h
        Gui/Tasks/TaskAddNewFiles.cpp
        # TaskAddNewFolders
        Gui/Tasks/TaskAddNewFolders.h
        Gui/Tasks/TaskAddNewFolders.cpp
        # TaskNavigateFileSystem
        Gui/Tasks/TaskNaviagateFileSystem.h
        Gui/Tasks/TaskNaviagateFileSystem.cpp
    #

    main.cpp

    Utility/DatabaseRegistry.h
    Utility/DatabaseRegistry.cpp

    Backend/FileStorageSubSystem/V2_FileStorageManager.h
    Backend/FileStorageSubSystem/V2_FileStorageManager.cpp

    # ORM
        # Repository
        Backend/FileStorageSubSystem/ORM/Repository/FolderRepository.h
        Backend/FileStorageSubSystem/ORM/Repository/FolderRepository.cpp
        Backend/FileStorageSubSystem/ORM/Repository/FileRepository.h
        Backend/FileStorageSubSystem/ORM/Repository/FileRepository.cpp
        Backend/FileStorageSubSystem/ORM/Repository/FileVersionRepository.h
        Backend/FileStorageSubSystem/ORM/Repository/FileVersionRepository.cpp

        # Entity
        Backend/FileStorageSubSystem/ORM/Entity/FolderEntity.h
        Backend/FileStorageSubSystem/ORM/Entity/FolderEntity.cpp
        Backend/FileStorageSubSystem/ORM/Entity/FileEntity.h
        Backend/FileStorageSubSystem/ORM/Entity/FileEntity.cpp
        Backend/FileStorageSubSystem/ORM/Entity/FileVersionEntity.h
        Backend/FileStorageSubSystem/ORM/Entity/FileVersionEntity.cpp
    #

    Backend/FileStorageSubSystem/SqlPrimitives/TableInfoFolderRecord.h
    Backend/FileStorageSubSystem/SqlPrimitives/TableInfoFileRecord.h
    Backend/FileStorageSubSystem/SqlPrimitives/TableInfoFileVersion.h
    Backend/FileStorageSubSystem/SqlPrimitives/TableInfoFileEvent.h
    Backend/FileStorageSubSystem/SqlPrimitives/TableInfoSaveGroupItem.h
    Backend/FileStorageSubSystem/SqlPrimitives/BaseSqlPrimitive.h Backend/FileStorageSubSystem/SqlPrimitives/BaseSqlPrimitive.cpp
    Backend/FileStorageSubSystem/SqlPrimitives/BaseRow.h Backend/FileStorageSubSystem/SqlPrimitives/BaseRow.cpp
    Backend/FileStorageSubSystem/SqlPrimitives/RowFolderRecord.h Backend/FileStorageSubSystem/SqlPrimitives/RowFolderRecord.cpp
    Backend/FileStorageSubSystem/SqlPrimitives/RowFileRecord.h Backend/FileStorageSubSystem/SqlPrimitives/RowFileRecord.cpp
    Backend/FileStorageSubSystem/SqlPrimitives/RowFileVersion.h Backend/FileStorageSubSystem/SqlPrimitives/RowFileVersion.cpp
    Backend/FileStorageSubSystem/SqlPrimitives/RowFileEvent.h Backend/FileStorageSubSystem/SqlPrimitives/RowFileEvent.cpp
    Backend/FileStorageSubSystem/SqlPrimitives/RowSaveGroupItem.h Backend/FileStorageSubSystem/SqlPrimitives/RowSaveGroupItem.cpp
    Backend/FileStorageSubSystem/SqlPrimitives/QueryFolderRecord.h Backend/FileStorageSubSystem/SqlPrimitives/QueryFolderRecord.cpp
    Backend/FileStorageSubSystem/SqlPrimitives/QueryFileRecord.h Backend/FileStorageSubSystem/SqlPrimitives/QueryFileRecord.cpp
    Backend/FileStorageSubSystem/SqlPrimitives/QueryFileVersion.h Backend/FileStorageSubSystem/SqlPrimitives/QueryFileVersion.cpp
    Backend/FileStorageSubSystem/SqlPrimitives/QueryFileEvent.h Backend/FileStorageSubSystem/SqlPrimitives/QueryFileEvent.cpp
    Backend/FileStorageSubSystem/SqlPrimitives/QuerySaveGroupItem.h Backend/FileStorageSubSystem/SqlPrimitives/QuerySaveGroupItem.cpp
    Backend/FileStorageSubSystem/SqlPrimitives/RowInserter.h Backend/FileStorageSubSystem/SqlPrimitives/RowInserter.cpp
    Backend/FileStorageSubSystem/SqlPrimitives/RowDeleter.h Backend/FileStorageSubSystem/SqlPrimitives/RowDeleter.cpp

    Backend/FileStorageSubSystem/RequestResults/FolderRequestResult.h Backend/FileStorageSubSystem/RequestResults/FolderRequestResult.cpp
    Backend/FileStorageSubSystem/RequestResults/FileRequestResult.h Backend/FileStorageSubSystem/RequestResults/FileRequestResult.cpp
    Backend/FileStorageSubSystem/RequestResults/FileVersionRequestResult.h Backend/FileStorageSubSystem/RequestResults/FileVersionRequestResult.cpp
    Backend/FileStorageSubSystem/RequestResults/SaveGroupItemMetaData.h Backend/FileStorageSubSystem/RequestResults/SaveGroupItemMetaData.cpp
    Backend/FileStorageSubSystem/FileStorageManager.h Backend/FileStorageSubSystem/FileStorageManager.cpp


    #[[ FileMonitoringSubSystem ]]
    Backend/FileMonitorSubSystem/FileSystemEventListener.h Backend/FileMonitorSubSystem/FileSystemEventListener.cpp
    Backend/FileMonitorSubSystem/FileSystemEventDb.h Backend/FileMonitorSubSystem/FileSystemEventDb.cpp
    Backend/FileMonitorSubSystem/FileMonitoringManager.h Backend/FileMonitorSubSystem/FileMonitoringManager.cpp

    #[[  ]]
    Backend/FileMonitorSubSystem/efsw/platform/platformimpl.hpp

    Backend/FileMonitorSubSystem/efsw/platform/posix/FileSystemImpl.cpp
    Backend/FileMonitorSubSystem/efsw/platform/posix/FileSystemImpl.hpp
    Backend/FileMonitorSubSystem/efsw/platform/posix/MutexImpl.cpp
    Backend/FileMonitorSubSystem/efsw/platform/posix/MutexImpl.hpp
    Backend/FileMonitorSubSystem/efsw/platform/posix/SystemImpl.cpp
    Backend/FileMonitorSubSystem/efsw/platform/posix/SystemImpl.hpp
    Backend/FileMonitorSubSystem/efsw/platform/posix/ThreadImpl.cpp
    Backend/FileMonitorSubSystem/efsw/platform/posix/ThreadImpl.hpp

    Backend/FileMonitorSubSystem/efsw/platform/win/FileSystemImpl.cpp
    Backend/FileMonitorSubSystem/efsw/platform/win/FileSystemImpl.hpp
    Backend/FileMonitorSubSystem/efsw/platform/win/MutexImpl.cpp
    Backend/FileMonitorSubSystem/efsw/platform/win/MutexImpl.hpp
    Backend/FileMonitorSubSystem/efsw/platform/win/SystemImpl.cpp
    Backend/FileMonitorSubSystem/efsw/platform/win/SystemImpl.hpp
    Backend/FileMonitorSubSystem/efsw/platform/win/ThreadImpl.cpp
    Backend/FileMonitorSubSystem/efsw/platform/win/ThreadImpl.hpp

    Backend/FileMonitorSubSystem/efsw/Atomic.hpp
    Backend/FileMonitorSubSystem/efsw/base.hpp
    Backend/FileMonitorSubSystem/efsw/Debug.hpp
    Backend/FileMonitorSubSystem/efsw/Debug.cpp
    Backend/FileMonitorSubSystem/efsw/DirectorySnapshot.hpp
    Backend/FileMonitorSubSystem/efsw/DirectorySnapshot.cpp
    Backend/FileMonitorSubSystem/efsw/DirectorySnapshotDiff.hpp
    Backend/FileMonitorSubSystem/efsw/DirectorySnapshotDiff.cpp
    Backend/FileMonitorSubSystem/efsw/DirWatcherGeneric.hpp
    Backend/FileMonitorSubSystem/efsw/DirWatcherGeneric.cpp
    Backend/FileMonitorSubSystem/efsw/FileInfo.hpp
    Backend/FileMonitorSubSystem/efsw/FileInfo.cpp
    Backend/FileMonitorSubSystem/efsw/FileSystem.hpp
    Backend/FileMonitorSubSystem/efsw/FileSystem.cpp
    Backend/FileMonitorSubSystem/efsw/FileWatcher.cpp
    Backend/FileMonitorSubSystem/efsw/FileWatcherCWrapper.cpp
    Backend/FileMonitorSubSystem/efsw/FileWatcherFSEvents.hpp
    Backend/FileMonitorSubSystem/efsw/FileWatcherFSEvents.cpp
    Backend/FileMonitorSubSystem/efsw/FileWatcherGeneric.hpp
    Backend/FileMonitorSubSystem/efsw/FileWatcherGeneric.cpp
    Backend/FileMonitorSubSystem/efsw/FileWatcherImpl.hpp
    Backend/FileMonitorSubSystem/efsw/FileWatcherImpl.cpp
    Backend/FileMonitorSubSystem/efsw/FileWatcherInotify.hpp
    Backend/FileMonitorSubSystem/efsw/FileWatcherInotify.cpp
    Backend/FileMonitorSubSystem/efsw/FileWatcherKqueue.hpp
    Backend/FileMonitorSubSystem/efsw/FileWatcherKqueue.cpp
    Backend/FileMonitorSubSystem/efsw/FileWatcherWin32.hpp
    Backend/FileMonitorSubSystem/efsw/FileWatcherWin32.cpp
    Backend/FileMonitorSubSystem/efsw/inotify-nosys.h
    Backend/FileMonitorSubSystem/efsw/Lock.hpp
    Backend/FileMonitorSubSystem/efsw/Log.cpp
    Backend/FileMonitorSubSystem/efsw/Mutex.hpp
    Backend/FileMonitorSubSystem/efsw/Mutex.cpp
    Backend/FileMonitorSubSystem/efsw/sophist.h
    Backend/FileMonitorSubSystem/efsw/String.hpp
    Backend/FileMonitorSubSystem/efsw/String.cpp
    Backend/FileMonitorSubSystem/efsw/System.hpp
    Backend/FileMonitorSubSystem/efsw/System.cpp
    Backend/FileMonitorSubSystem/efsw/Thread.hpp
    Backend/FileMonitorSubSystem/efsw/Thread.cpp
    Backend/FileMonitorSubSystem/efsw/Utf.hpp
    Backend/FileMonitorSubSystem/efsw/Utf.inl
    Backend/FileMonitorSubSystem/efsw/Watcher.hpp
    Backend/FileMonitorSubSystem/efsw/Watcher.cpp
    Backend/FileMonitorSubSystem/efsw/WatcherFSEvents.hpp
    Backend/FileMonitorSubSystem/efsw/WatcherFSEvents.cpp
    Backend/FileMonitorSubSystem/efsw/WatcherGeneric.hpp
    Backend/FileMonitorSubSystem/efsw/WatcherGeneric.cpp
    Backend/FileMonitorSubSystem/efsw/WatcherInotify.hpp
    Backend/FileMonitorSubSystem/efsw/WatcherInotify.cpp
    Backend/FileMonitorSubSystem/efsw/WatcherKqueue.hpp
    Backend/FileMonitorSubSystem/efsw/WatcherKqueue.cpp
    Backend/FileMonitorSubSystem/efsw/WatcherWin32.hpp
    Backend/FileMonitorSubSystem/efsw/WatcherWin32.cpp

    Backend/FileMonitorSubSystem/efsw/efsw.h
    Backend/FileMonitorSubSystem/efsw/efsw.hpp

    Backend/FileMonitorSubSystem/efsw/Test/UpdateListener.h
    Backend/FileMonitorSubSystem/efsw/Test/UpdateListener.cpp

    #[[ Global scope ]]

    resources.qrc

)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(BoldBackup
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET BoldBackup APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(BoldBackup SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(BoldBackup
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(BoldBackup PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Sql Qt${QT_VERSION_MAJOR}::Concurrent Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(BoldBackup PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(BoldBackup)
endif()
