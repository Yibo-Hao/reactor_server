cmake_minimum_required(VERSION 3.28)
project(reactor)

set(CMAKE_CXX_STANDARD 17)

add_executable(server src/server.cpp)
add_executable(client src/client.cpp)

add_subdirectory(lib/inet_address)
add_subdirectory(lib/socket)
add_subdirectory(lib/epoll)
target_link_libraries(server inet_address socket epoll)