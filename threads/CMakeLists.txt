
add_executable ( simple simple.cpp )
target_link_libraries( simple PUBLIC pthread )

add_executable ( affinity affinity.cpp )
target_link_libraries( affinity PUBLIC pthread )

add_executable ( queue_bench_v1 queue_bench_v1_mutex.cpp )
target_link_libraries( queue_bench_v1 PUBLIC pthread )

add_executable ( queue_bench_v1a queue_bench_v1a_mutex_drain.cpp )
target_link_libraries( queue_bench_v1a PUBLIC pthread )

add_executable ( queue_bench_v1b queue_bench_v1b_mutex_circular.cpp )
target_link_libraries( queue_bench_v1b PUBLIC pthread )

add_executable ( queue_bench_v1c queue_bench_v1c_mutex_drain_circular.cpp )
target_link_libraries( queue_bench_v1c PUBLIC pthread )



add_executable ( queue_bench_v2 queue_bench_v2_spinlock.cpp )
target_link_libraries( queue_bench_v2 PUBLIC pthread )

add_executable ( queue_bench_v2a queue_bench_v2a_spinlock_drain.cpp )
target_link_libraries( queue_bench_v2a PUBLIC pthread )

add_executable ( queue_bench_v2b queue_bench_v2b_spinlock_circular.cpp )
target_link_libraries( queue_bench_v2b PUBLIC pthread )

add_executable ( queue_bench_v2c queue_bench_v2c_spinlock_drain_circular.cpp )
target_link_libraries( queue_bench_v2c PUBLIC pthread )



add_executable ( queue_bench_v3 queue_bench_v3_boost_lockfree.cpp )
target_link_libraries( queue_bench_v3 PUBLIC pthread ${Boost_LIBRARIES} )

add_executable ( queue_bench_v3a queue_bench_v3a_boost_lockfree_drain.cpp )
target_link_libraries( queue_bench_v3a PUBLIC pthread ${Boost_LIBRARIES} )



add_executable ( circular_buffer boost_circular_buffer_example.cpp )
target_link_libraries( circular_buffer PUBLIC pthread ${Boost_LIBRARIES} )

