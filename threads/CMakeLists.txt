
add_executable (simple simple.cpp)
target_link_libraries(simple PUBLIC pthread)

add_executable (affinity affinity.cpp)
target_link_libraries(affinity PUBLIC pthread)

