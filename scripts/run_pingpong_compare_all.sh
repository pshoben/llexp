#!/bin/bash
#set -v

export EXE_DIR=../build/clocks/
export BENCHMARK_BINARY1=bench_pingpong_rdtsc
export BENCHMARK_BINARY2=bench_pingpong_rdtscp

function run_with_threads {
   
    nice -n -20 ${EXE_DIR}/${BENCHMARK_BINARY1} -s $1
    nice -n -20 ${EXE_DIR}/${BENCHMARK_BINARY2} -s $1

}

COL_HEADER="|option ---|cpu|step ------ | blocksize -| count ---- | avg (cyc) -| avg (ns) - | mad (cyc)- | mad (ns) - | min (cyc)- | min (ns) - | max (cyc)- | max (ns) - |"
 SEPARATOR="|----------|---|------------|------------|------------|------------|------------|------------|------------|------------|------------|------------|------------|"
#            |          1 |          1 |         16 |  259039223 |      188.5 |       78.7 |      128.2 |       53.5 

echo ""
echo "# Running PingPong Benchmark (${BENCHMARK_BINARY1} ${BENCHMARK_BINARY2})"
echo ""

echo "All test runs performed on [Target Hardware](target_architecture.md)"

echo ""
echo "\`\`\`"
echo ""
echo "Run #1"
echo ""
echo $COL_HEADER
echo $SEPARATOR

for x in {1..16} 
do
run_with_threads $x
done

echo ""
echo "Run #2"
echo ""
echo $COL_HEADER
echo $SEPARATOR

for x in {1..16} 
do
run_with_threads $x
done

echo "\`\`\`"
echo "done"


