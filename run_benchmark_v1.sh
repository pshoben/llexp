#!/bin/bash
#set -v

function run_with_threads {
   
    nice -n -20 build/threads/queue_bench  -m 100 -r 10 -n $1
    nice -n -20 build/threads/queue_bench  -m 500 -r 50 -n $1
    nice -n -20 build/threads/queue_bench  -m 1000 -r 100 -n $1
    nice -n -20 build/threads/queue_bench  -m 2500 -r 250 -n $1
    nice -n -20 build/threads/queue_bench  -m 5000 -r 500 -n $1
    nice -n -20 build/threads/queue_bench  -m 10000 -r 1000 -n $1
    nice -n -20 build/threads/queue_bench  -m 25000 -r 2500 -n $1
    nice -n -20 build/threads/queue_bench  -m 50000 -r 5000 -n $1
    nice -n -20 build/threads/queue_bench  -m 100000 -r 10000 -n $1
    nice -n -20 build/threads/queue_bench  -m 100000 -r 100000 -n $1
    nice -n -20 build/threads/queue_bench  -m 1000000 -r 1000000 -n $1

    echo ""
}

COL_HEADER="|num threads| rate - msg/sec | total msg count | latency avg (ns) | latency stddev ns| latency sigma ns | latency 2sigma ns | latency 3sigma ns | hist 0123456789ABCDEFGHIJKLMNOPQRSTUV"
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

run_with_threads 4 
run_with_threads 2 

echo ""
echo "## Run #2"
echo ""
echo $COL_HEADER
echo $SEPARATOR

run_with_threads 4 
run_with_threads 2 


echo "done"

