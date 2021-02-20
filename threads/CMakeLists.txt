
add_executable (simple simple.cpp)
target_link_libraries(simple PUBLIC pthread)

add_executable (affinity affinity.cpp)
target_link_libraries(affinity PUBLIC pthread)

add_executable (queue_bench queue_bench.cpp)
target_link_libraries(queue_bench PUBLIC pthread)

add_executable (queue_bench_v2 queue_bench_v2_pthread_spinlock_std_queue.cpp)
target_link_libraries(queue_bench_v2 PUBLIC pthread)

