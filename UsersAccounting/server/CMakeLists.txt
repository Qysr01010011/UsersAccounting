add_library(${PROJECT_NAME}-server-static STATIC
    src/controllers/ServerManager.cpp
    src/repository/DatabaseRepository.cpp
)

target_include_directories(${PROJECT_NAME}-server-static PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/headers>
)

target_link_libraries(${PROJECT_NAME}-server-static PRIVATE
        SQLite::SQLite3
        JsonCpp::JsonCpp
)

add_executable(${PROJECT_NAME}-server server.cpp)

target_link_libraries(${PROJECT_NAME}-server PRIVATE
        ${PROJECT_NAME}-server-static
        Drogon::Drogon
)

install(TARGETS ${PROJECT_NAME}-server RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})