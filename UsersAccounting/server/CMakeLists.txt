if(APPLE OR WIN32)
    set(SERVER_LIB_TYPE SHARED)
    set(SERVER_LIB_SUFFIX shared)
else ()
    set(SERVER_LIB_TYPE STATIC)
    set(SERVER_LIB_SUFFIX static)
endif ()


add_library(${PROJECT_NAME}-server-${SERVER_LIB_SUFFIX} ${SERVER_LIB_TYPE}
    src/controllers/ServerManager.cpp
    src/repository/DatabaseRepository.cpp
)

set_target_properties(${PROJECT_NAME}-server-${SERVER_LIB_SUFFIX} PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_include_directories(${PROJECT_NAME}-server-${SERVER_LIB_SUFFIX} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/headers>
    $<INSTALL_INTERFACE:include/${PROJECT_NAME}/server>
)

target_link_libraries(${PROJECT_NAME}-server-${SERVER_LIB_SUFFIX} PRIVATE
        SQLite::SQLite3
        JsonCpp::JsonCpp
)

add_executable(${PROJECT_NAME}-server server.cpp)

target_link_libraries(${PROJECT_NAME}-server PRIVATE
        ${PROJECT_NAME}-server-${SERVER_LIB_SUFFIX}
        Drogon::Drogon
)

if(WIN32)
    target_link_libraries(${PROJECT_NAME}-server PRIVATE ws2_32)
    target_compile_definitions(${PROJECT_NAME}-server PRIVATE _WIN32_WINNT=0x0A00)
endif ()

install(TARGETS ${PROJECT_NAME}-server ${PROJECT_NAME}-server-${SERVER_LIB_SUFFIX}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY headers DESTINATION include/${PROJECT_NAME}/server)