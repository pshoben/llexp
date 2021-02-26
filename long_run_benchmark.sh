#!/bin/bash
#set -v

function run_with_threads {
   
    nice -n -20 build/threads/queue_bench_v${1} -v -m 50000000 -r 5000 -n $2
}

COL_HEADER="|num threads| rate (msgs/sec)| total msg count | latency avg (ns) | latency stddev ns| latency sigma ns | latency 2sigma ns | latency 3sigma ns | hist 0123456789ABCDEFGHIJKLMNOPQRSTUV"
SEPARATOR="|-----------|----------------|-----------------|------------------|------------------|------------------|-------------------|-------------------|--------------------------------------"

run_with_threads $1 2

