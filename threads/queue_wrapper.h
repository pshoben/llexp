#pragma once

#include <queue>
#include <time.h>

struct QueueWrapper {
public:
 std::queue<uint64_t> queue;
 void write() {
    struct timespec write_time;
    clock_gettime( CLOCK_MONOTONIC, &write_time );
    uint64_t val=(uint64_t) write_time.tv_nsec;
    queue.push(val);
 }

 void read() {
   uint64_t write_time = queue.front();
   queue.pop();
   struct timespec read_time;
   clock_gettime( CLOCK_MONOTONIC, &read_time );
   int64_t queue_latency_ns = read_time.tv_nsec - write_time;
   std::cout << " queue latency (ns) = " << queue_latency_ns << "\n";
 } 
};

