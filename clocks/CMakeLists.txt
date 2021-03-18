
#add_executable ( bench_simple_rdtsc bench_simple.cpp )
#target_compile_options( bench_simple_rdtsc PRIVATE -g -O3 -Wall -Wpedantic -Wextra -Wshadow -fno-omit-frame-pointer -std=c++2a --save-temps -DUSE_RDTSC)
#target_link_libraries( bench_simple_rdtsc PRIVATE pthread )
#
#add_executable ( bench_simple_rdtscp bench_simple.cpp )
#target_compile_options( bench_simple_rdtscp PRIVATE -g -O3 -Wall -Wpedantic -Wextra -Wshadow -fno-omit-frame-pointer -std=c++2a --save-temps -DUSE_RDTSCP)
#target_link_libraries( bench_simple_rdtscp PRIVATE pthread )

add_executable ( bench_pingpong_rdtsc bench_pingpong_simple.cpp )
target_compile_options( bench_pingpong_rdtsc PRIVATE -g -O3 -Wall -Wpedantic -Wextra -Wshadow -fno-omit-frame-pointer -std=c++2a --save-temps -DUSE_RDTSC)
target_link_libraries( bench_pingpong_rdtsc PRIVATE pthread )

add_executable ( bench_pingpong_rdtscp bench_pingpong_simple.cpp )
target_compile_options( bench_pingpong_rdtscp PRIVATE -g -O3 -Wall -Wpedantic -Wextra -Wshadow -fno-omit-frame-pointer -std=c++2a --save-temps -DUSE_RDTSCP)
target_link_libraries( bench_pingpong_rdtscp PRIVATE pthread )

add_executable ( bench_pingpong_wait bench_pingpong_wait.cpp )
target_compile_options( bench_pingpong_wait PRIVATE -g -O3 -Wall -Wpedantic -Wextra -Wshadow -fno-omit-frame-pointer -std=c++2a --save-temps -DUSE_RDTSCP ) # -march=cascadelake) #  -mcldemote )
target_link_libraries( bench_pingpong_wait PRIVATE pthread )
#
#add_executable ( bench_pingpong_atomic bench_pingpong_atomic.cpp )
#target_compile_options( bench_pingpong_atomic PRIVATE -g -O3 -Wall -Wpedantic -Wextra -Wshadow -fno-omit-frame-pointer -std=c++2a --save-temps -DUSE_RDTSCP)
#target_link_libraries( bench_pingpong_atomic PRIVATE pthread )

add_executable ( bench_pingpong_asm bench_pingpong_asm.cpp )
target_compile_options( bench_pingpong_asm PRIVATE -g -O3 -Wall -Wpedantic -Wextra -Wshadow -fno-omit-frame-pointer -std=c++2a --save-temps -DUSE_RDTSCP ) # -march=cascadelake ) # -mcldemote )
target_link_libraries( bench_pingpong_asm PRIVATE pthread )

