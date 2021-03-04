
#set_target_properties( bench_simple_rdtsc PROPERTIES COMPILE_OPTIONS -DUSE_RDTSC)
add_executable ( bench_simple_rdtsc bench_rdtsc.cpp )
target_link_libraries( bench_simple_rdtsc PRIVATE pthread )

#set_target_properties( bench_simple_rdtscp PROPERTIES COMPILE_OPTIONS -DUSE_RDTSCP)
add_executable ( bench_simple_rdtscp bench_rdtscp.cpp )
target_link_libraries( bench_simple_rdtscp PRIVATE pthread )

