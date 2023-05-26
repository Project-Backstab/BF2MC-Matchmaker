cmake_minimum_required(VERSION 3.16.3)

project(bf2mc)

set (CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")

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
	src/battlefield/playerstats.cpp
	src/main.cpp
)

include_directories(bf2mc src src/include)

target_link_libraries (bf2mc mysqlclient ssl crypto zstd)