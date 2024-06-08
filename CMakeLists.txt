cmake_minimum_required(VERSION 3.16.3)

option(JSONCPP_WITH_TESTS "Compile and (for jsoncpp_check) run JsonCpp test executables" OFF)
option(BUILD_SHARED_LIBS "Build jsoncpp_lib as a shared library." ON)
option(BUILD_STATIC_LIBS "Build jsoncpp_lib as a static library." ON)
option(BUILD_OBJECT_LIBS "Build jsoncpp_lib as a object library." OFF)
option (ATOMIZES_ENABLE_TESTS "Build tests for atomizes?" OFF)

add_subdirectory(third-party/atomizes)
add_subdirectory(third-party/jsoncpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
set(CMAKE_CXX_COMPILER "ccache g++")

## Debug
#set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -O2 -g")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -g")

## Release
set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -s -O2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s -O2")

## BF2MC-Matchmaker
project(bf2mc VERSION 1.0.0)

## Executable
add_executable(bf2mc
    src/server.cpp
    src/gpsp/client.cpp
    src/gpcm/client.cpp
    src/webserver/client.cpp
    src/webserver/api.cpp
    src/browsing/client.cpp
    src/browsing/sb_crypt.cpp
    src/browsing/test.cpp
    src/gamestats/client.cpp
    src/gamestats/test.cpp
    src/websocket/client.cpp
    src/service/file_system.cpp
    src/qr/client.cpp
    src/net/socket.cpp
    src/database.cpp
    src/database/chat.cpp
    src/database/clan_rank.cpp
    src/database/clan.cpp
    src/database/game_server_player.cpp
    src/database/game_server.cpp
    src/database/game_stat_player.cpp
    src/database/game_stat.cpp
    src/database/leaderboard_clan.cpp
    src/database/leaderboard_rank.cpp
    src/database/leaderboard_ratio.cpp
    src/database/leaderboard_type.cpp
    src/database/player_friend.cpp
    src/database/player_stat.cpp
    src/database/player.cpp
    src/util.cpp
    src/logger.cpp
    src/gamespy.cpp
    src/battlefield.cpp
    src/battlefield/player.cpp
    src/battlefield/playerstats.cpp
    src/battlefield/clan.cpp
    src/battlefield/gameserver.cpp
    src/battlefield/gamestat.cpp
    src/main.cpp
)

include_directories(bf2mc src)
target_link_libraries(bf2mc mysqlclient ssl crypto jsoncpp_static atomizes)

## Version
execute_process(
    COMMAND git rev-parse --show-toplevel
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE PROJECT_GIT_TOPLEVEL
    OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(
    COMMAND basename "${PROJECT_GIT_TOPLEVEL}"
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE PROJECT_GIT_NAME
    OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(
    COMMAND git rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE PROJECT_GIT_BRANCH_NAME
    OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(
    COMMAND git rev-parse HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE PROJECT_GIT_BRANCH_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(
    COMMAND date
    OUTPUT_VARIABLE PROJECT_COMPILE_DATE
    OUTPUT_STRIP_TRAILING_WHITESPACE)

configure_file(src/version.h.in ${CMAKE_SOURCE_DIR}/src/version.h @ONLY)

## Doxygen documentation
find_package(Doxygen REQUIRED)

set(DOXYGEN_INPUT ${CMAKE_SOURCE_DIR}/src)
set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/doc)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)

add_custom_target(
    doc
    COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen"
    VERBATIM
)

## Installation
install(TARGETS bf2mc DESTINATION bin)
install(
    DIRECTORY ${CMAKE_SOURCE_DIR}/data/
    DESTINATION data
    FILES_MATCHING PATTERN "*.*")
