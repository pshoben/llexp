
add_executable ( bench_2w1r_boost_lockfree bench_2w1r_boost_lockfree.cpp)
target_link_libraries( bench_2w1r_boost_lockfree PUBLIC pthread ${Boost_LIBRARIES} )

add_executable ( bench_2w1r_boost_spsc_lockfree bench_2w1r_boost_spsc_lockfree.cpp)
target_link_libraries( bench_2w1r_boost_spsc_lockfree PUBLIC pthread ${Boost_LIBRARIES} )

