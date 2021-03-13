
add_executable ( bench_simple_rdtsc bench_simple.cpp )
target_compile_options( bench_simple_rdtsc PRIVATE -g -O3 -Wall -Wpedantic -Wextra -Wshadow -fno-omit-frame-pointer -std=c++2a -DUSE_RDTSC)
target_link_libraries( bench_simple_rdtsc PRIVATE pthread )

add_executable ( bench_simple_rdtscp bench_simple.cpp )
target_compile_options( bench_simple_rdtscp PRIVATE -g -O3 -Wall -Wpedantic -Wextra -Wshadow -fno-omit-frame-pointer -std=c++2a -DUSE_RDTSCP)
target_link_libraries( bench_simple_rdtscp PRIVATE pthread )

add_executable ( bench_pingpong_rdtsc bench_pingpong_simple.cpp )
target_compile_options( bench_pingpong_rdtsc PRIVATE -g -O3 -Wall -Wpedantic -Wextra -Wshadow -fno-omit-frame-pointer -std=c++2a -DUSE_RDTSC)
target_link_libraries( bench_pingpong_rdtsc PRIVATE pthread )

add_executable ( bench_pingpong_rdtscp bench_pingpong_simple.cpp )
target_compile_options( bench_pingpong_rdtscp PRIVATE -g -O3 -Wall -Wpedantic -Wextra -Wshadow -fno-omit-frame-pointer -std=c++2a -DUSE_RDTSCP)
target_link_libraries( bench_pingpong_rdtscp PRIVATE pthread )

