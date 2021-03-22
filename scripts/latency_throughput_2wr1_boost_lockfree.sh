#!/bin/bash
#set -v
export EXE_DIR=../build/queue/

COL_HEADER="| name ---- | cpus | tot msg |targ m/s |actl m/s | lat med ns | lat 90p ns | lat 95p ns | lat 99p ns |lat 99.5p ns|lat 99.7p ns|lat 99.9p ns| lat max ns | throughput msg/sec log2 hist --- | latency log2 hist"
 SEPARATOR="| ----------|------|---------|---------|---------|------------|------------|------------|------------|------------|------------|------------|------------|----------------------------------|----------------------------"

function run_with_threads {
 
    echo $COL_HEADER
    echo $SEPARATOR

    cd $EXE_DIR

    ./bench_2w1r_boost_lockfree -r 500
    ./bench_2w1r_boost_lockfree -r 1250
    ./bench_2w1r_boost_lockfree -r 2500
    ./bench_2w1r_boost_lockfree -r 3750

    ./bench_2w1r_boost_lockfree -r 5000
    ./bench_2w1r_boost_lockfree -r 12500
    ./bench_2w1r_boost_lockfree -r 25000
    ./bench_2w1r_boost_lockfree -r 37500

    ./bench_2w1r_boost_lockfree -r 50000
    ./bench_2w1r_boost_lockfree -r 125000
    ./bench_2w1r_boost_lockfree -r 250000
    ./bench_2w1r_boost_lockfree -r 375000

    ./bench_2w1r_boost_lockfree -r 500000
    ./bench_2w1r_boost_lockfree -r 1250000
    ./bench_2w1r_boost_lockfree -r 2500000
    ./bench_2w1r_boost_lockfree -r 3750000


#    ./bench_2w1r_boost_lockfree -r 2500
#    ./bench_2w1r_boost_lockfree -r 5000
#    ./bench_2w1r_boost_lockfree -r 7500
#
#    ./bench_2w1r_boost_lockfree -r 10000
#    ./bench_2w1r_boost_lockfree -r 25000
#    ./bench_2w1r_boost_lockfree -r 50000
#    ./bench_2w1r_boost_lockfree -r 75000

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

run_with_threads  

echo ""
echo "\`\`\`"
echo "done"


