cmake_minimum_required (VERSION 3.11)
project(llexp)

set (CMAKE_CXX_STANDARD 20)

add_executable(tcp_relay llnet/tcp_relay.cpp)
target_link_libraries(tcp_relay rt)

add_subdirectory(llpool)