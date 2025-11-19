set(CMAKE_AUTOMOC ON)

add_library(${PROJECT_NAME}-client-shared SHARED
        src/ui/MainUi.cpp
        src/ui/NewUserUi.cpp
        src/UsersWindow.cpp
        src/AddUserDialog.cpp
        src/server/ServerConnection.cpp
        src/view/UsersViewModel.cpp
)

target_include_directories(${PROJECT_NAME}-client-shared PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/headers>
    $<INSTALL_INTERFACE:include>
)

target_sources(${PROJECT_NAME}-client-shared PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/headers/AddUserDialog.h
        ${CMAKE_CURRENT_SOURCE_DIR}/headers/UsersWindow.h
        ${CMAKE_CURRENT_SOURCE_DIR}/headers/view/UsersViewModel.h
)

target_link_libraries(${PROJECT_NAME}-client-shared PUBLIC
        Qt5::Widgets
        Qt5::WebSockets
)

add_executable(${PROJECT_NAME}-client src/client.cpp
        headers/utils/enums.h
        headers/utils/enumWrapper.h)

target_link_libraries(${PROJECT_NAME}-client PUBLIC
        ${PROJECT_NAME}-client-shared
)

install(TARGETS ${PROJECT_NAME}-client ${PROJECT_NAME}-client-shared
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)