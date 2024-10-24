add_library(socket socket.cpp socket.h)
target_link_libraries(socket inet_address)

target_include_directories(socket PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})