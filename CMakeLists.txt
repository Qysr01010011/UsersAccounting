cmake_minimum_required(VERSION 3.16)
project(users-accounting VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_MACOSX_RPATH ON)
set(CMAKE_BUILD_WITH_INSTALL_RPATH OFF)

include(GNUInstallDirs)

find_package(Qt6 COMPONENTS Core Widgets Sql WebSockets QUIET)

if(NOT Qt6_FOUND)
    find_package(Qt5 COMPONENTS Core Widgets Sql WebSockets REQUIRED)
    set(QT_PREFIX Qt5)
else ()
    set(QT_PREFIX Qt6)
endif ()

find_package(SQLite3 REQUIRED)
find_package(jsoncpp CONFIG REQUIRED)
find_package(Drogon CONFIG REQUIRED)

add_subdirectory(UsersAccounting)

include(InstallRequiredSystemLibraries)  # добавляет libc и другие стандартные зависимости

set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_CONTACT "sav.bennington@gmail.com")

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "Configuring CPack for Linux (DEB)")
    set(CPACK_GENERATOR "DEB")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Qysr01010011")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.31), libstdc++6 (>= 10), sqlite3, sqlite3-doc, drogon, jsoncpp")

    set(CPACK_RPM_PACKAGE_LICENSE "MIT")
    set(CPACK_RPM_PACKAGE_REQUIRES "glibc >= 2.31, libstdc++ >= 10, sqlite3, drogon, jsoncpp")

elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(STATUS "Configuring CPack for Windows (NSIS)")
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)

elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    message(STATUS "Configuring CPack for macOS (DragNDrop)")
    set(CPACK_GENERATOR "DragNDrop")

else()
    message(WARNING "Unsupported system: ${CMAKE_SYSTEM_NAME}. No installer will be generated.")
endif()

include(CPack)