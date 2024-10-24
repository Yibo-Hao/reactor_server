add_library(epoll epoll.cpp epoll.h)

target_include_directories(epoll PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})