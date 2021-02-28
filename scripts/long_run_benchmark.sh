#!/bin/bash
#set -v

export EXE_DIR=../build/threads/
export BENCHMARK_BINARY=queue_bench_v${1}


function run_with_threads {
 
    nice -n -20 ${EXE_DIR}/${BENCHMARK_BINARY} -v -m 50000000 -r 5000 -n $1
}

COL_HEADER="|num threads| rate (msgs/sec)| total msg count | latency avg (ns) | latency stddev ns| latency sigma ns | latency 2sigma ns | latency 3sigma ns | hist 0123456789ABCDEFGHIJKLMNOPQRSTUV"
SEPARATOR="|-----------|----------------|-----------------|------------------|------------------|------------------|-------------------|-------------------|--------------------------------------"

run_with_threads 2

