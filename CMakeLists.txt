cmake_minimum_required (VERSION 3.11)
project(llexp)

FIND_PACKAGE( Boost 1.66 COMPONENTS program_options system REQUIRED )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

INCLUDE_DIRECTORIES( common )

#set (CMAKE_CXX_STANDARD 2a)

#set (CMAKE_CXX_FLAGS " -std=c++2a -Wall -Wextra -Wpedantic -Wshadow -fno-omit-frame-pointer -O2 -Wno-unused -fsanitize=address ")
set (CMAKE_CXX_FLAGS " -g -std=c++2a -Wall -Wextra -Wpedantic -Wshadow -fno-omit-frame-pointer -O3 -Wno-vla ") # --save-temps ")


add_executable(tcp_relay llnet/tcp_relay.cpp)
#target_link_libraries(tcp_relay rt asan)
target_link_libraries(tcp_relay rt)

add_subdirectory(llpool)

add_subdirectory(threads)
add_subdirectory(clocks)



