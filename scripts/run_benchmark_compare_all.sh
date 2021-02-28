#!/bin/bash
#set -v
export EXE_DIR=../build/threads/

COL_HEADER="| name ---- | cpus | rate - msg/sec | total msg count | lat med ns | lat 90p ns | lat 95p ns | lat 99p ns |lat 99.5p ns|lat 99.7p ns| lat max ns | 0123456789ABCDEFGHIJKLMNOPQRSTUVXYZ log2 hist"
 SEPARATOR="| ----------|------|----------------|-----------------|------------|------------|------------|------------|------------|------------|------------|----------------------------------------------"

function run_compare {

    nice -n -20 ${EXE_DIR}/queue_bench_v1  -m $1 -r $2 -n $3
    nice -n -20 ${EXE_DIR}/queue_bench_v1a -m $1 -r $2 -n $3
    nice -n -20 ${EXE_DIR}/queue_bench_v1b -m $1 -r $2 -n $3
    nice -n -20 ${EXE_DIR}/queue_bench_v1c -m $1 -r $2 -n $3

    nice -n -20 ${EXE_DIR}/queue_bench_v2  -m $1 -r $2 -n $3
    nice -n -20 ${EXE_DIR}/queue_bench_v2a -m $1 -r $2 -n $3
    nice -n -20 ${EXE_DIR}/queue_bench_v2b -m $1 -r $2 -n $3
    nice -n -20 ${EXE_DIR}/queue_bench_v2c -m $1 -r $2 -n $3

    nice -n -20 ${EXE_DIR}/queue_bench_v3  -m $1 -r $2 -n $3
    nice -n -20 ${EXE_DIR}/queue_bench_v3a  -m $1 -r $2 -n $3

    echo ""
}

function run_with_threads {
 
    echo $COL_HEADER
    echo $SEPARATOR
   
    run_compare 100         10 $1
#    run_compare 250         25 $1
#    run_compare 500         50 $1
#    run_compare 750         75 $1
    run_compare 1000       100 $1
#    run_compare 2500       250 $1
#    run_compare 5000       500 $1
#    run_compare 7500       750 $1
    run_compare 10000     1000 $1
    run_compare 25000     2500 $1
    run_compare 50000     5000 $1
    run_compare 75000     7500 $1
    run_compare 100000   10000 $1
#    run_compare 250000   25000 $1
#    run_compare 500000   50000 $1
#    run_compare 750000   75000 $1

    run_compare 1000000 100000 $1
#    run_compare 12500000 1250000 $1
#    run_compare 15000000 1500000 $1
#    run_compare 17500000 1750000 $1
#    run_compare 20000000 2000000 $1
    echo ""
}


echo ""
echo "# Running Queue Latency Microbenchmark : side-by-side compare algorithms"
echo ""
echo "All test runs performed on [Target Hardware](target_architecture.md)"
echo ""
echo "\`\`\`"
echo ""

run_with_threads 4 
run_with_threads 2 

echo ""
echo "\`\`\`"
echo "done"


