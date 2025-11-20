set(CMAKE_AUTOMOC ON)

add_library(${PROJECT_NAME}-client-shared SHARED
        src/ui/MainUi.cpp
        src/ui/NewUserUi.cpp
        src/UsersWindow.cpp
        src/AddUserDialog.cpp
        src/server/ServerConnection.cpp
        src/view/UsersViewModel.cpp

        headers/AddUserDialog.h
        headers/UsersWindow.h
        headers/server/ServerConnection.h
        headers/view/UsersViewModel.h
)

target_include_directories(${PROJECT_NAME}-client-shared PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/headers>
)

target_link_libraries(${PROJECT_NAME}-client-shared PUBLIC
        Qt5::Widgets
        Qt5::WebSockets
)

add_executable(${PROJECT_NAME}-client src/client.cpp)

target_link_libraries(${PROJECT_NAME}-client PUBLIC
        ${PROJECT_NAME}-client-shared
)

install(TARGETS ${PROJECT_NAME}-client ${PROJECT_NAME}-client-shared LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})