#pragma once

#include <queue>
#include <mutex>
#include <cstring>
#include "common.h"

struct QueueWrapper {
public:
 std::queue<struct timespec> queue;
 std::mutex queue_mutex;
 const struct timespec null_time{0,0};
 
 static const size_t MAX_SAMPLES = 1000000U;


















 struct timespec write() 
 {
    queue_mutex.lock();

    struct timespec write_time;
    clock_gettime( CLOCK_MONOTONIC, &write_time );
    queue.push( write_time );
    queue_mutex.unlock();
    return write_time;
 }

 TimespecPair * read( TimespecPair * next_sample ) 
 {
   queue_mutex.lock();
   if( !queue.empty()) {

     next_sample->write_time = queue.front();
     queue.pop();
     queue_mutex.unlock();

     clock_gettime( CLOCK_MONOTONIC, &next_sample->read_time );
     next_sample++;

   } else {
     queue_mutex.unlock();
   }
   return next_sample;
 } 

};

