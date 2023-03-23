cmake_minimum_required(VERSION 3.5)

project(NeSync VERSION 0.1 LANGUAGES CXX)

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
include_directories(Backend/FileStorageSubSystem/)
include_directories(Backend/FileStorageSubSystem/ORM/)
include_directories(Backend/FileMonitorSubSystem/)

add_subdirectory(Dependency/efsw)
add_subdirectory(Dependency/quazip)


set(PROJECT_SOURCES
    Gui/MainWindow.h Gui/MainWindow.cpp Gui/MainWindow.ui

    # BaseDialog
        Gui/Dialogs/BaseDialog.h
        Gui/Dialogs/BaseDialog.cpp
    #

    # DialogAddNewFolder
        Gui/Dialogs/DialogAddNewFolder.h
        Gui/Dialogs/DialogAddNewFolder.cpp
        Gui/Dialogs/DialogAddNewFolder.ui
        #
        Gui/DataModels/DialogAddNewFolder/CustomFileSystemModel.h
        Gui/DataModels/DialogAddNewFolder/CustomFileSystemModel.cpp
    #

    # DialogEditVersion
        Gui/Dialogs/DialogEditVersion.h
        Gui/Dialogs/DialogEditVersion.cpp
        Gui/Dialogs/DialogEditVersion.ui
    #

    # DialogCreateCopy
        Gui/Dialogs/DialogCreateCopy.h
        Gui/Dialogs/DialogCreateCopy.cpp
        Gui/Dialogs/DialogCreateCopy.ui
    #

    # DialogExport
        Gui/Dialogs/DialogExport.h
        Gui/Dialogs/DialogExport.cpp
        Gui/Dialogs/DialogExport.ui
    #

    # DialogImport
        Gui/Dialogs/DialogImport.h
        Gui/Dialogs/DialogImport.cpp
        Gui/Dialogs/DialogImport.ui
        #
        Gui/DataModels/DialogImport/TreeItem.h
        Gui/DataModels/DialogImport/TreeItem.cpp
        #
        Gui/DataModels/DialogImport/TreeModelDialogImport.h
        Gui/DataModels/DialogImport/TreeModelDialogImport.cpp
        #
        Gui/DataModels/DialogImport/ItemDelegateAction.h
        Gui/DataModels/DialogImport/ItemDelegateAction.cpp
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
        Gui/DataModels/TabFileMonitor/TreeItem.h
        Gui/DataModels/TabFileMonitor/TreeItem.cpp
        Gui/DataModels/TabFileMonitor/TreeModelFileMonitor.h
        Gui/DataModels/TabFileMonitor/TreeModelFileMonitor.cpp
        Gui/DataModels/TabFileMonitor/ItemDelegateAction.h
        Gui/DataModels/TabFileMonitor/ItemDelegateAction.cpp
        Gui/DataModels/TabFileMonitor/ItemDelegateDescription.h
        Gui/DataModels/TabFileMonitor/ItemDelegateDescription.cpp
        #
    #

    # DialogDebugFileMonitor
        Gui/Dialogs/DialogDebugFileMonitor.h
        Gui/Dialogs/DialogDebugFileMonitor.cpp
        Gui/Dialogs/DialogDebugFileMonitor.ui
    #

    # Tasks
        # TaskAddNewFolders
        Gui/Tasks/TaskAddNewFolders.h
        Gui/Tasks/TaskAddNewFolders.cpp
        # TaskSaveChanges
        Gui/Tasks/TaskSaveChanges.h
        Gui/Tasks/TaskSaveChanges.cpp
    #

    main.cpp

    Utility/DatabaseRegistry.h
    Utility/DatabaseRegistry.cpp
    Utility/JsonDtoFormat.h

    Backend/FileStorageSubSystem/FileStorageManager.h
    Backend/FileStorageSubSystem/FileStorageManager.cpp

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

    # FileMonitoringSubSystem
        Backend/FileMonitorSubSystem/FileSystemEventListener.h
        Backend/FileMonitorSubSystem/FileSystemEventListener.cpp
        Backend/FileMonitorSubSystem/FileSystemEventDb.h
        Backend/FileMonitorSubSystem/FileSystemEventDb.cpp
        Backend/FileMonitorSubSystem/FileMonitoringManager.h
        Backend/FileMonitorSubSystem/FileMonitoringManager.cpp
    #
)


if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(NeSync
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET NeSync APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(NeSync SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(NeSync
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(NeSync PRIVATE Qt${QT_VERSION_MAJOR}::Core
                             PRIVATE Qt${QT_VERSION_MAJOR}::Sql
                             PRIVATE Qt${QT_VERSION_MAJOR}::Concurrent
                             PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
                             QuaZip::QuaZip
                             efsw)


set_target_properties(NeSync PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(NeSync)
endif()
