#!/bin/bash
#set -v
export EXE_DIR=../build/queue/

COL_HEADER="| name ---- | cpus | tot msg |targ m/s |actl m/s | lat med ns | lat 90p ns | lat 95p ns | lat 99p ns |lat 99.5p ns|lat 99.7p ns|lat 99.9p ns| lat max ns | throughput msg/sec log2 hist --- | latency log2 hist"
 SEPARATOR="| ----------|------|---------|---------|---------|------------|------------|------------|------------|------------|------------|------------|------------|----------------------------------|----------------------------"

function run_with_threads {
 
    ./bench_2w1r_boost_lockfree -r $1
    ./bench_2w1r_boost_spsc_lockfree -r $1
}

function run_compare {
 
    echo $COL_HEADER
    echo $SEPARATOR

    cd $EXE_DIR

    run_with_threads 2500
    run_with_threads 3750

    run_with_threads 5000
    run_with_threads 12500
    run_with_threads 25000
    run_with_threads 37500

    run_with_threads 50000
    run_with_threads 125000
    run_with_threads 250000
    run_with_threads 375000

    run_with_threads 500000
    run_with_threads 1250000
    run_with_threads 2500000
    run_with_threads 3750000

    cd -
    echo ""
}


echo ""
echo "# Running Latency/Throughput Microbenchmark : boost::lockfree:queue : 2 writers -> 1 reader "
echo ""
echo "All test runs performed on [Target Hardware](target_architecture.md)"
echo ""
echo "\`\`\`"
echo ""

run_compare  

echo ""
echo "\`\`\`"
echo "done"


