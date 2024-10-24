cmake_minimum_required(VERSION 3.28)
project(reactor)

set(CMAKE_CXX_STANDARD 17)

add_executable(reactor main.cpp)
add_executable(server server.cpp)
add_executable(client client.cpp)

add_subdirectory(lib)
target_link_libraries(server inet_address)