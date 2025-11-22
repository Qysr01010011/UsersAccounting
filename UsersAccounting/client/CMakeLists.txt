if(APPLE OR WIN32)
    set(CLIENT_LIB_TYPE SHARED)
    set(CLIENT_LIB_SUFFIX shared)
else ()
    set(CLIENT_LIB_TYPE STATIC)
    set(CLIENT_LIB_SUFFIX static)
endif ()


add_library(${PROJECT_NAME}-client-${CLIENT_LIB_SUFFIX} SHARED
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

target_include_directories(${PROJECT_NAME}-client-${CLIENT_LIB_SUFFIX} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/headers>
    $<INSTALL_INTERFACE:include/${PROJECT_NAME}/client>
)

target_link_libraries(${PROJECT_NAME}-client-${CLIENT_LIB_SUFFIX} PUBLIC
        ${QT_PREFIX}::Widgets
        ${QT_PREFIX}::WebSockets
)

if(WIN32)
    add_executable(${PROJECT_NAME}-client WIN32 src/client.cpp)
else ()
    add_executable(${PROJECT_NAME}-client src/client.cpp)
endif ()

target_link_libraries(${PROJECT_NAME}-client PUBLIC
        ${PROJECT_NAME}-client-${CLIENT_LIB_SUFFIX}
)

if(APPLE)
    set_target_properties(${PROJECT_NAME}-client PROPERTIES
            MACOSX_BUNDLE TRUE
            MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist
    )
endif ()

install(TARGETS ${PROJECT_NAME}-client ${PROJECT_NAME}-client-${CLIENT_LIB_SUFFIX}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY headers DESTINATION include/${PROJECT_NAME}/client)