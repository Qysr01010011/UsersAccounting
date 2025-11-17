cmake_minimum_required(VERSION 3.28)
project(users-accounting)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)

include(GNUInstallDirs)
set(CMAKE_INSTALL_PREFIX "/usr/local")

find_package(Qt5 COMPONENTS
        Core
        Widgets
        Sql
        WebSockets
        REQUIRED)

find_package(SQLite3 REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

include(FetchContent)
FetchContent_Declare(
        ixwebsocket
        GIT_REPOSITORY https://github.com/machinezone/IXWebSocket.git
        GIT_TAG master
)

FetchContent_MakeAvailable(ixwebsocket)

add_subdirectory(UsersAccounting)