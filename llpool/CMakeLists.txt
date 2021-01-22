# CMakeList.txt : CMake project for StringBenchmarks, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.11)

set (CMAKE_CXX_STANDARD 20)

# Include Google Benchmark
include(../cmake/GoogleBenchmark.cmake)

add_executable (arena_harness
  "arena_harness.cpp" "arena_pool.cpp"
)

add_executable (rule_loader_harness
  "rule_loader.cpp"
)

add_executable (C2Benchmarks
  "C2Benchmarks.cpp"
)
target_link_libraries(C2Benchmarks benchmark::benchmark)


add_executable (StringBenchmarks
  "StringBenchmarks.cpp"
  "StringBenchmarks.h"
)
target_link_libraries(StringBenchmarks benchmark::benchmark)

add_executable (UnorderedMap
  "UnorderedMap.cpp"
  "StringBenchmarks.h"
)
target_link_libraries(UnorderedMap benchmark::benchmark)


# Add source to this project's executable.
add_executable (Harness
  "harness.cpp"
)

# Link Shlwapi to the project
if ("${CMAKE_SYSTEM_NAME}" MATCHES "Windows")
    target_link_libraries(StringBenchmarks Shlwapi)
    target_link_libraries(Harness Shlwapi)
endif()

# TODO: Add tests and install targets if needed.
