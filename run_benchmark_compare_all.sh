#!/bin/bash
#set -v

export BENCHMARK_BINARY=queue_bench_v1

function run_compare {
    nice -n -20 build/threads/queue_bench_v1  -m $1 -r $2 -n $3
    nice -n -20 build/threads/queue_bench_v2  -m $1 -r $2 -n $3
    echo ""
}

function run_with_threads {
   
    run_compare 100 10 $1
    run_compare 250 25 $1
    run_compare 500 50 $1
    run_compare 750 75 $1
    run_compare 1000 100 $1
    run_compare 2500 250 $1
   #run_compare 4000 400 $1
    run_compare 5000 500 $1
    run_compare 7500 750 $1
    run_compare 10000 1000 $1
    run_compare 25000 2500 $1
    run_compare 50000 5000 $1
    run_compare 75000 7500 $1
    run_compare 100000 10000 $1
    run_compare 250000 25000 $1
    run_compare 500000 50000 $1
    run_compare 750000 750000 $1
    run_compare 1000000 1000000 $1
    run_compare 1250000 1250000 $1
    run_compare 1500000 1500000 $1
    run_compare 1750000 1750000 $1
    run_compare 2000000 2000000 $1
    echo ""
}


COL_HEADER="| name | cpus | rate - msg/sec | total msg count | lat med ns | lat 90p ns | lat 95p ns | lat 99p ns |lat 99.5p ns|lat 99.7p ns| lat max ns | 0123456789ABCDEFGHIJKLMNOPQRSTUVXYZ log2 hist"
 SEPARATOR="| -----|------|----------------|-----------------|------------|------------|------------|------------|------------|------------|------------|----------------------------------------------"



#COL_HEADER="|num threads| rate - msg/sec | total msg count | latency avg (ns) | latency stddev ns| latency sigma ns | latency 2sigma ns | latency 3sigma ns | hist 0123456789ABCDEFGHIJKLMNOPQRSTUV"
#SEPARATOR="|-----------|----------------|-----------------|------------------|------------------|------------------|-------------------|-------------------|--------------------------------------"

echo ""
echo "# Running Queue Latency Benchmark #1 (algorithm = std::queue with std::mutex )"
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


