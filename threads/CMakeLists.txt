
add_executable (simple simple.cpp)
target_link_libraries(simple PUBLIC pthread)

add_executable (affinity affinity.cpp)
target_link_libraries(affinity PUBLIC pthread)

add_executable (queue_bench_v1 queue_bench_v1_mutex.cpp)
target_link_libraries(queue_bench_v1 PUBLIC pthread)

add_executable (queue_bench_v2 queue_bench_v2_spinlock.cpp)
target_link_libraries(queue_bench_v2 PUBLIC pthread)

