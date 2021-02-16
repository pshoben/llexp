#!/bin/bash
#set -v

# usage perf_run_*.sh num-threads (usually 2 or 4)

function run_with_threads {
   
    build/threads/queue_bench  -m 5000000 -r 5000 -n $1
}

COL_HEADER="|num threads| rate (msgs/sec)| total msg count | latency avg (ns) | latency stddev ns| latency sigma ns | latency 2sigma ns | latency 3sigma ns | hist 0123456789ABCDEFGHIJKLMNOPQRSTUV"
SEPARATOR="|-----------|----------------|-----------------|------------------|------------------|------------------|-------------------|-------------------|--------------------------------------"
echo ""
echo "# Running Queue Latency Benchmark #1 (algorithm = std::queue with std::mutex)"
echo ""
lscpu

echo ""
echo "## Run #1"
echo ""
echo $COL_HEADER
echo $SEPARATOR

run_with_threads $1 

echo "done"

