include(CheckIncludeFileCXX)

check_include_file_cxx(any HAS_ANY)
check_include_file_cxx(string_view HAS_STRING_VIEW)
check_include_file_cxx(coroutine HAS_COROUTINE)

if (NOT "${CMAKE_CXX_STANDARD}" STREQUAL "")
    # Do nothing
elseif (HAS_ANY AND HAS_STRING_VIEW AND HAS_COROUTINE)
    set(CMAKE_CXX_STANDARD 20)
elseif (HAS_ANY AND HAS_STRING_VIEW)
    set(CMAKE_CXX_STANDARD 17)
else ()
    set(CMAKE_CXX_STANDARD 14)
endif ()

add_library(${PROJECT_NAME}-server-static STATIC
    src/controllers/ServerManager.cpp
    src/repository/DatabaseRepository.cpp
)

target_include_directories(${PROJECT_NAME}-server-static PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/headers>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(${PROJECT_NAME}-server-static PRIVATE
        SQLite::SQLite3
        nlohmann_json::nlohmann_json
        JsonCpp::JsonCpp
)

add_executable(${PROJECT_NAME}-server server.cpp)

target_link_libraries(${PROJECT_NAME}-server PRIVATE
        ${PROJECT_NAME}-server-static
        Drogon::Drogon
)

# ##############################################################################

if (CMAKE_CXX_STANDARD LESS 17)
    message(FATAL_ERROR "c++17 or higher is required")
elseif (CMAKE_CXX_STANDARD LESS 20)
    message(STATUS "use c++17")
else ()
    message(STATUS "use c++20")
endif ()

aux_source_directory(src/controllers CTL_SRC)

install(TARGETS ${PROJECT_NAME}-server ${PROJECT_NAME}-server-static
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
