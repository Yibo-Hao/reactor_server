cmake_minimum_required(VERSION 3.28)
project(reactor)

set(CMAKE_CXX_STANDARD 17)

add_executable(src/server server.cpp)
add_executable(src/client client.cpp)

add_subdirectory(lib/inet_address)
target_link_libraries(server inet_address)