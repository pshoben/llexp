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

#run_with_threads 8
#run_with_threads 6 
run_with_threads 4 
run_with_threads 2 

echo "done"
