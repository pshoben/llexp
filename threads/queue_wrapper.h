#pragma once

#include <queue>
//#include <list>
//#include <stdint.h>
#include <mutex>
//#include <vector>
#include <cstring>
#include "common.h"

struct QueueWrapper {
public:
  std::queue<struct timespec> queue;
  std::mutex queue_mutex;

  static const size_t MAX_SAMPLES = 1000000U;
  //std::vector<TimespecPair> samples{ MAX_SAMPLES };

 
 struct timespec write() {
    std::unique_lock<std::mutex> guard( queue_mutex );
    struct timespec write_time;
    clock_gettime( CLOCK_MONOTONIC, &write_time );
    //std::cout << "writer got time : " << write_time.tv_sec << "." << write_time.tv_nsec << "\n";
    //uint64_t val=(uint64_t) write_time.tv_nsec;
    queue.push( write_time );
    return write_time;
 }

 TimespecPair * read(TimespecPair * next_sample) {
   std::unique_lock<std::mutex> guard( queue_mutex );
   if( !queue.empty()) {
     struct timespec write_time = queue.front();
     queue.pop();
     struct timespec read_time;
     clock_gettime( CLOCK_MONOTONIC, &read_time );
     //std::cout << "reader got write time : " << write_time.tv_sec << "." << write_time.tv_nsec << "\n";
     //std::cout << "reader got time : " << read_time.tv_sec << "." << read_time.tv_nsec << "\n";
 
     //samples.emplace_back( write_time, read_time ); 

     // record the raw sample
     next_sample->write_time = write_time;
     next_sample->read_time = read_time;

     // increment only after store
     next_sample++;

     //int64_t queue_latency_ns = timespec_diff_ns( write_time, read_time );
     //std::cout << " queue latency (ns) = " << queue_latency_ns << "\n";
   }
   return next_sample;
 } 
};

