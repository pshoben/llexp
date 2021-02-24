#!/bin/bash
#set -v

# usage:  perf_record_benchmark v1

function fecho {
    echo $1 >> perf_run.out
}

function run_with_threads {
   
    nice -n -20 build/threads/queue_bench_v${1}  -m 5000000 -r 5000 -n $1 2>&1 >> perf_run.out & export PID=$(echo $!) 
    echo "got PID=$PID"
    sleep 1
    ps -leaf | egrep 'PID|queue_bench' | grep -v grep
    echo "recording perf stats for 30 seconds..."
    perf record -s -F 99 -p $PID -g -- sleep 30
    perf report
}

COL_HEADER="|num threads| rate (msgs/sec)| total msg count | latency avg (ns) | latency stddev ns| latency sigma ns | latency 2sigma ns | latency 3sigma ns | hist 0123456789ABCDEFGHIJKLMNOPQRSTUV"
SEPARATOR="|-----------|----------------|-----------------|------------------|------------------|------------------|-------------------|-------------------|--------------------------------------"
fecho ""
fecho "# Running Queue Latency Benchmark #1 (algorithm = std::queue with std::mutex)"
fecho ""
lscpu >> perf_run.out

fecho ""
fecho "## Run #1"
fecho ""
fecho $COL_HEADER
fecho $SEPARATOR

run_with_threads 2

