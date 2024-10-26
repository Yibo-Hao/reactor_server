cmake_minimum_required(VERSION 3.28)
project(reactor)

set(CMAKE_CXX_STANDARD 17)

add_executable(client src/client.cpp)

add_executable(server src/server.cpp
		lib/channel.cpp
		lib/epoll.cpp
		lib/socket.cpp
		lib/inetAddress.cpp
		lib/eventLoop.cpp
)
target_include_directories(server PRIVATE include)