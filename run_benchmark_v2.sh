#!/bin/bash
#set -v

export BENCHMARK_BINARY=queue_bench_v2

function run_with_threads {
   
    nice -n -20 build/threads/${BENCHMARK_BINARY}  -m 100 -r 10 -n $1
    nice -n -20 build/threads/${BENCHMARK_BINARY}  -m 250 -r 25 -n $1
    nice -n -20 build/threads/${BENCHMARK_BINARY}  -m 500 -r 50 -n $1
    nice -n -20 build/threads/${BENCHMARK_BINARY}  -m 750 -r 75 -n $1
    nice -n -20 build/threads/${BENCHMARK_BINARY}  -m 1000 -r 100 -n $1
    nice -n -20 build/threads/${BENCHMARK_BINARY}  -m 2500 -r 250 -n $1
    nice -n -20 build/threads/${BENCHMARK_BINARY}  -m 4000 -r 400 -n $1
    nice -n -20 build/threads/${BENCHMARK_BINARY}  -m 5000 -r 500 -n $1
    nice -n -20 build/threads/${BENCHMARK_BINARY}  -m 7500 -r 750 -n $1
    nice -n -20 build/threads/${BENCHMARK_BINARY}  -m 10000 -r 1000 -n $1
    nice -n -20 build/threads/${BENCHMARK_BINARY}  -m 25000 -r 2500 -n $1
    nice -n -20 build/threads/${BENCHMARK_BINARY}  -m 50000 -r 5000 -n $1
    nice -n -20 build/threads/${BENCHMARK_BINARY}  -m 75000 -r 7500 -n $1
    nice -n -20 build/threads/${BENCHMARK_BINARY}  -m 100000 -r 10000 -n $1
    nice -n -20 build/threads/${BENCHMARK_BINARY}  -m 100000 -r 100000 -n $1
    nice -n -20 build/threads/${BENCHMARK_BINARY}  -m 1000000 -r 1000000 -n $1

    echo ""
}

#COL_HEADER="|num threads| rate - msg/sec | total msg count | latency avg (ns) | latency stddev ns| latency sigma ns | latency 2sigma ns | latency 3sigma ns | hist 0123456789ABCDEFGHIJKLMNOPQRSTUV"
# SEPARATOR="|-----------|----------------|-----------------|------------------|------------------|------------------|-------------------|-------------------|--------------------------------------"

COL_HEADER="|num threads| rate - msg/sec | total msg count | lat med ns | lat 90p ns | lat 95p ns | lat 99p ns |lat 99.5p ns|lat 99.7p ns| lat max ns | 0123456789ABCDEFGHIJKLMNOPQRSTUVXYZ log2 hist"
 SEPARATOR="|-----------|----------------|-----------------|------------|------------|------------|------------|------------|------------|------------|----------------------------------------------"


echo ""
echo "# Running Queue Latency Benchmark #2 (algorithm = std::queue with pthread_spinlock )"
echo ""

echo "All test runs performed on [Target Hardware](target_architecture.md)"

echo ""
echo "\`\`\`"
echo ""
echo "Run #1"
echo ""
echo $COL_HEADER
echo $SEPARATOR

run_with_threads 4 
run_with_threads 2 

echo ""
echo "Run #2"
echo ""
echo $COL_HEADER
echo $SEPARATOR

run_with_threads 4 
run_with_threads 2 

echo "\`\`\`"
echo "done"


