cmake_minimum_required(VERSION 3.14)

project(nesync LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Sql HttpServer Core5Compat)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Sql HttpServer Core5Compat)

include_directories(Utility/)
include_directories(FileStorageSubSystem/)
include_directories(FileStorageSubSystem/ORM/)
include_directories(RestApi/)
include_directories(RestApi/Services)

add_subdirectory(Dependency/quazip)


set(PROJECT_SOURCES
  main.cpp

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

  # Rest Api
      # Services
      RestApi/Services/ZipExportService.h
      RestApi/Services/ZipExportService.cpp
      RestApi/Services/ZipImportService.h
      RestApi/Services/ZipImportService.cpp
      RestApi/Services/FileStorageService.h
      RestApi/Services/FileStorageService.cpp
      RestApi/Services/FileSystemMonitorService.h
      RestApi/Services/FileSystemMonitorService.cpp

      # Controllers
      RestApi/ZipExportController.h
      RestApi/ZipExportController.cpp
      RestApi/ZipImportController.h
      RestApi/ZipImportController.cpp
      RestApi/FileStorageController.h
      RestApi/FileStorageController.cpp
      RestApi/FileSystemMonitorController.h
      RestApi/FileSystemMonitorController.cpp
  #
)

if (WIN32)
    enable_language("RC")
    set(app_icon_resource_windows "${CMAKE_CURRENT_SOURCE_DIR}/Resources/res_win.rc")
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(nesync
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${app_icon_resource_windows}
    )
endif()

target_link_libraries(nesync PRIVATE Qt${QT_VERSION_MAJOR}::Core
                                    PRIVATE Qt${QT_VERSION_MAJOR}::Sql
                                    PRIVATE Qt${QT_VERSION_MAJOR}::HttpServer
                                    PRIVATE Qt${QT_VERSION_MAJOR}::Core5Compat
                                    QuaZip::QuaZip)

# This version info is required for MacOS compilation
# Windows compilation uses /Resources/res_win.rc

set (PROJECT_VERSION 2.0)

set_target_properties(nesync PROPERTIES
    # MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    # MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    # MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE FALSE # https://cmake.org/cmake/help/latest/prop_tgt/MACOSX_BUNDLE.html
    WIN32_EXECUTABLE FALSE # https://cmake.org/cmake/help/latest/prop_tgt/WIN32_EXECUTABLE.html
)

include(GNUInstallDirs)
install(TARGETS nesync
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    BUNDLE  DESTINATION ${CMAKE_SOURCE_DIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(nesync)
endif()
