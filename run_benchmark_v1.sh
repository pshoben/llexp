#!/bin/bash
#set -v

function run_with_threads {
    build/threads/queue_bench  -m 1000000 -r 1000000 -n $1
    build/threads/queue_bench  -m 100000 -r 100000 -n $1
    build/threads/queue_bench  -m 100000 -r 10000 -n $1
    build/threads/queue_bench  -m 50000 -r 5000 -n $1
    build/threads/queue_bench  -m 10000 -r 1000 -n $1
    build/threads/queue_bench  -m 5000 -r 500 -n $1
    build/threads/queue_bench  -m 2500 -r 250 -n $1
    build/threads/queue_bench  -m 1000 -r 100 -n $1
    build/threads/queue_bench  -m 500 -r 50 -n $1
    build/threads/queue_bench  -m 100 -r 10 -n $1
}

# column headers:
COL_HEADER="|num threads| rate (msgs/sec)| total msg count | latency avg (ns) | latency stddev ns| latency sigma ns | latency 2sigma ns | latency 3sigma ns | hist 0123456789ABCDEFGHIJKLMNOPQRSTUV"
SEPARATOR="|-----------|----------------|-----------------|------------------|------------------|------------------|-------------------|-------------------|--------------------------------------"
echo $COL_HEADER
echo $SEPARATOR

#"threads  4 | rate   1000000 | count   1000000 | avg      1035.86 | dev      22040.1 | sig      23075.9 | 2sig        45116 | 3sig      67156.1 | hist .......EIIHFA762334565

#run_with_threads 8
#run_with_threads 6 
run_with_threads 4 
run_with_threads 2 

echo "done"
