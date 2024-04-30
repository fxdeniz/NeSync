cmake_minimum_required(VERSION 3.14)

project(NeSync_Server LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Sql Concurrent HttpServer)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Sql Concurrent HttpServer)

include_directories(Utility/)
include_directories(FileStorageSubSystem/)
include_directories(FileStorageSubSystem/ORM/)
include_directories(FileMonitorSubSystem/)

add_subdirectory(Dependency/efsw)

set(PROJECT_SOURCES
  main.cpp
  resources.qrc

  Utility/DatabaseRegistry.h
  Utility/DatabaseRegistry.cpp
  Utility/AppConfig.h
  Utility/AppConfig.cpp
  Utility/JsonDtoFormat.h

  FileStorageSubSystem/FileStorageManager.h
  FileStorageSubSystem/FileStorageManager.cpp

  # ORM
      # Repository
      FileStorageSubSystem/ORM/Repository/FolderRepository.h
      FileStorageSubSystem/ORM/Repository/FolderRepository.cpp
      FileStorageSubSystem/ORM/Repository/FileRepository.h
      FileStorageSubSystem/ORM/Repository/FileRepository.cpp
      FileStorageSubSystem/ORM/Repository/FileVersionRepository.h
      FileStorageSubSystem/ORM/Repository/FileVersionRepository.cpp

      # Entity
      FileStorageSubSystem/ORM/Entity/FolderEntity.h
      FileStorageSubSystem/ORM/Entity/FolderEntity.cpp
      FileStorageSubSystem/ORM/Entity/FileEntity.h
      FileStorageSubSystem/ORM/Entity/FileEntity.cpp
      FileStorageSubSystem/ORM/Entity/FileVersionEntity.h
      FileStorageSubSystem/ORM/Entity/FileVersionEntity.cpp
  #

  # FileMonitoringSubSystem
      FileMonitorSubSystem/FileSystemEventListener.h
      FileMonitorSubSystem/FileSystemEventListener.cpp
      FileMonitorSubSystem/FileSystemEventStore.h
      FileMonitorSubSystem/FileSystemEventStore.cpp
  #
)

if (WIN32)
    enable_language("RC")
    set(app_icon_resource_windows "${CMAKE_CURRENT_SOURCE_DIR}/Resources/res_win.rc")
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(NeSync_Server
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${app_icon_resource_windows}
    )
endif()

target_link_libraries(NeSync_Server PRIVATE Qt${QT_VERSION_MAJOR}::Core
                                    PRIVATE Qt${QT_VERSION_MAJOR}::Sql
                                    PRIVATE Qt${QT_VERSION_MAJOR}::Concurrent
                                    PRIVATE Qt${QT_VERSION_MAJOR}::HttpServer
                                    efsw)

# This version info is required for MacOS compilation
# Windows compilation uses /Resources/res_win.rc

set (PROJECT_VERSION 2.0)

set_target_properties(NeSync_Server PROPERTIES
    # MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    # MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    # MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    # MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS NeSync_Server
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(NeSync_Server)
endif()
