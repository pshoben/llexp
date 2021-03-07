
#set (TARGET1_CXX_FLAGS " -g -std=c++2a -Wall -Wextra -Wpedantic -Wshadow -fno-omit-frame-pointer -O3 -Wno-vla --save-temps -DUSE_RDTSC ")
add_executable ( bench_simple_rdtsc bench_rdtsc.cpp )
target_link_libraries( bench_simple_rdtsc PRIVATE pthread )
#set_target_properties( bench_simple_rdtsc PROPERTIES COMPILE_FLAGS ${TARGET1_CXX_FLAGS} )


#set (TARGET2_CXX_FLAGS " -g -std=c++2a -Wall -Wextra -Wpedantic -Wshadow -fno-omit-frame-pointer -O3 -Wno-vla --save-temps -DUSE_RDTSCP ")
add_executable ( bench_simple_rdtscp bench_rdtscp.cpp )
target_link_libraries( bench_simple_rdtscp PRIVATE pthread )
#set_target_properties( bench_simple_rdtscp PROPERTIES COMPILE_FLAGS ${TARGET2_CXX_FLAGS})

