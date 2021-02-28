#!/bin/bash
#set -v

# usage:  perf_record_benchmark v1

export EXE_DIR=../build/threads/
export BENCHMARK_BINARY=queue_bench_v${1}
export PERFTOOLS_DIR=../../FlameGraph
function fecho {
    echo $1 >> perf_run.out
}

function run_with_threads {
   
    nice -n -20 ${EXE_DIR}/${BENCHMARK_BINARY}  -m 1000000 -r 5000 -n $1 2>&1 >> perf_run.out & export PID=$(echo $!) 
    echo "got PID=$PID"
    sleep 1
    ps -leaf | egrep 'PID|queue_bench' | grep -v grep

    echo "perf stat CPU 1,2 for 10 seconds..."
    timeout -s 2 5 perf stat -d --cpu 1,2 -A
 
    echo "perf record for 10 seconds..."
    echo "writing cpu call graph to call_graph.out"
 
    perf record -s -F 99 -p $PID -g -- sleep 10
   
    perf script | ${PERFTOOLS_DIR}/stackcollapse-perf.pl > out.perf-folded

   make_call_graph.py out.perf-folded > call_graph.out

    echo "stopping $PID"
    kill $PID

    perf report
}

COL_HEADER="|num threads| rate (msgs/sec)| total msg count | latency avg (ns) | latency stddev ns| latency sigma ns | latency 2sigma ns | latency 3sigma ns | hist 0123456789ABCDEFGHIJKLMNOPQRSTUV"
SEPARATOR="|-----------|----------------|-----------------|------------------|------------------|------------------|-------------------|-------------------|--------------------------------------"
fecho ""
fecho "# Running Queue Latency Benchmark #1 (algorithm = std::queue with std::mutex)"
fecho ""
lscpu >> perf_run.out

fecho $COL_HEADER
fecho $SEPARATOR

run_with_threads 2

