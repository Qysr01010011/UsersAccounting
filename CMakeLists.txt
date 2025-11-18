cmake_minimum_required(VERSION 3.28)
project(users-accounting LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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
find_package(jsoncpp CONFIG REQUIRED)
find_package(Drogon CONFIG REQUIRED)

add_subdirectory(UsersAccounting)