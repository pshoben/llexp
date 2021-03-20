#!/bin/bash
#set -v

export EXE_DIR=../build/clocks/
export BENCHMARK_BINARY1=bench_pingpong_asm
export BENCHMARK_BINARY2=bench_pingpong_rdtscp

function run_with_threads {
   
    nice -n -20 ${EXE_DIR}/${BENCHMARK_BINARY1} -s $1
    nice -n -20 ${EXE_DIR}/${BENCHMARK_BINARY2} -s $1
    nice -n -20 ${EXE_DIR}/${BENCHMARK_BINARY2} -s $1
    nice -n -20 ${EXE_DIR}/${BENCHMARK_BINARY1} -s $1


}

COL_HEADER="| option ---|cpu|step ------ | blocksize -| count ---- | num > 1 us | avg (cyc) -| avg (ns) - | mad (cyc)- | mad (ns) - | min (cyc)- | min (ns) - | max (cyc)- | max (ns) - |"
 SEPARATOR="|-----------|---|------------|------------|------------|------------|------------|------------|------------|------------|------------|------------|------------|------------|"

echo ""
echo "# Running PingPong Benchmark (${BENCHMARK_BINARY1} vs ${BENCHMARK_BINARY2})"
echo ""

echo "All test runs performed on [Target Hardware](target_architecture.md)"

echo ""
echo "\`\`\`"
echo $COL_HEADER
echo $SEPARATOR

for x in `seq 13 8 61`
do
run_with_threads $x
done

echo "\`\`\`"
echo "done"


