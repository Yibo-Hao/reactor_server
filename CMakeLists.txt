cmake_minimum_required(VERSION 3.28)
project(reactor)

set(CMAKE_CXX_STANDARD 17)

add_executable(client src/client.cpp)

add_executable(server
		${PROJECT_SOURCE_DIR}/src/server.cpp
		${PROJECT_SOURCE_DIR}/core/channel.cpp
		${PROJECT_SOURCE_DIR}/core/eventLoop.cpp
		${PROJECT_SOURCE_DIR}/core/epoll.cpp
		${PROJECT_SOURCE_DIR}/core/tcpServer.cpp
		${PROJECT_SOURCE_DIR}/core/inetAddress.cpp
		${PROJECT_SOURCE_DIR}/core/socket.cpp
		${PROJECT_SOURCE_DIR}/core/acceptor.cpp
		${PROJECT_SOURCE_DIR}/core/connection.cpp
)

target_include_directories(server PRIVATE include)