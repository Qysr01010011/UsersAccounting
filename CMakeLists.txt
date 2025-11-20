cmake_minimum_required(VERSION 3.16)
project(users-accounting LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(GNUInstallDirs)
set(CMAKE_INSTALL_PREFIX "/usr/local")

find_package(Qt5 COMPONENTS
        Core
        Widgets
        Sql
        WebSockets
        REQUIRED)

find_package(SQLite3 REQUIRED)
find_package(jsoncpp CONFIG REQUIRED)
find_package(Drogon CONFIG REQUIRED)

add_subdirectory(UsersAccounting)

include(InstallRequiredSystemLibraries)  # добавляет libc и другие стандартные зависимости

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "Configuring CPack for Linux (DEB)")
    set(CPACK_GENERATOR "DEB")
    set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
    set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
    set(CPACK_PACKAGE_CONTACT "sav.bennington@gmail.com")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Qysr01010011")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.31), libstdc++6 (>= 10), sqlite3, sqlite3-doc, drogon, jsoncpp")

    set(CPACK_RPM_PACKAGE_LICENSE "MIT")
    set(CPACK_RPM_PACKAGE_REQUIRES "glibc >= 2.31, libstdc++ >= 10, sqlite3, drogon, jsoncpp")

elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(STATUS "Configuring CPack for Windows (NSIS)")
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
    set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
    set(CPACK_PACKAGE_CONTACT "sav.bennington@gmail.com")

elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    message(STATUS "Configuring CPack for macOS (DragNDrop)")
    set(CPACK_GENERATOR "DragNDrop")
    set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
    set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
    set(CPACK_PACKAGE_CONTACT "sav.bennington@gmail.com")

else()
    message(WARNING "Unsupported system: ${CMAKE_SYSTEM_NAME}. No installer will be generated.")
endif()

include(CPack)