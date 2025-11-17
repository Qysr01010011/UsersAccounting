add_library(${PROJECT_NAME}-server-shared SHARED
        src/server-manager/ServerManager.cpp
        src/repository/DatabaseRepository.cpp
        src/utils/utils.cpp
)

target_include_directories(${PROJECT_NAME}-server-shared PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/headers>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(${PROJECT_NAME}-server-shared
        SQLite::SQLite3
        nlohmann_json::nlohmann_json
        ixwebsocket
)

add_executable(${PROJECT_NAME}-server src/server.cpp)

target_link_libraries(${PROJECT_NAME}-server ${PROJECT_NAME}-server-shared)

install(TARGETS ${PROJECT_NAME}-server ${PROJECT_NAME}-server-shared
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES database/source.sqlite DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/database)

