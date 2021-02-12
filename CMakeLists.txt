cmake_minimum_required (VERSION 3.11)
project(llexp)

#set (CMAKE_CXX_STANDARD 2a)

set (CMAKE_CXX_FLAGS " -std=c++2a -Wall -Wextra -Wpedantic -Wshadow -fno-omit-frame-pointer -O2 -Wno-unused ")

add_executable(tcp_relay llnet/tcp_relay.cpp)
#target_link_libraries(tcp_relay rt)

add_subdirectory(llpool)

add_subdirectory(threads)



