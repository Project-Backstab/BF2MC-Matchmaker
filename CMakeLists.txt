cmake_minimum_required(VERSION 3.16.3)

option(JSONCPP_WITH_TESTS "Compile and (for jsoncpp_check) run JsonCpp test executables" OFF)
option(BUILD_SHARED_LIBS "Build jsoncpp_lib as a shared library." ON)
option(BUILD_STATIC_LIBS "Build jsoncpp_lib as a static library." OFF)
option(BUILD_OBJECT_LIBS "Build jsoncpp_lib as a object library." OFF)

add_subdirectory(third-party/jsoncpp)

set (CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")

project(bf2mc)
add_executable(bf2mc
	src/server.cpp
	src/gpsp/client.cpp
	src/gpcm/client.cpp
	src/webserver/client.cpp
	src/browsing/client.cpp
	src/net/socket.cpp
	src/database.cpp
	src/util.cpp
	src/gamespy.cpp
	src/urlrequest.cpp
	src/battlefield/player.cpp
	src/battlefield/playerstats.cpp
	src/battlefield/clan.cpp
	src/main.cpp
)

include_directories(bf2mc src src/include)

target_link_libraries(bf2mc mysqlclient ssl crypto jsoncpp_lib)