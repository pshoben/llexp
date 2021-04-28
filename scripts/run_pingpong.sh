#!/bin/bash
#set -v

export EXE_DIR=../build/clocks/
#export BENCHMARK_BINARY=bench_pingpong_rdtsc
export BENCHMARK_BINARY=bench_pingpong_rdtscp

function run_with_threads {

#    nice -n -20 ../build/clocks/bench_pingpong_rdtscp -s 13  # ( block size = 64 ) --> no false sharing , no load splitting
   
    nice -n -20 ${EXE_DIR}/${BENCHMARK_BINARY} -s $1 -v
}

COL_HEADER="|option ---|cpu|step ------ | blocksize -| count ---- | avg (cyc) -| avg (ns) - | mad (cyc)- | mad (ns) - | min (cyc)- | min (ns) - | max (cyc)- | max (ns) - |"
 SEPARATOR="|----------|---|------------|------------|------------|------------|------------|------------|------------|------------|------------|------------|------------|"
#

echo ""
echo "# Running PingPong Benchmark (${BENCHMARK_BINARY})"
echo ""

echo "All test runs performed on [Target Hardware](target_architecture.md)"

echo ""
echo "\`\`\`"
echo ""
echo "Run #1"
echo ""
echo $COL_HEADER
echo $SEPARATOR

for x in `seq 13 8 61`
do
run_with_threads $x
done

echo "\`\`\`"
echo "done"


