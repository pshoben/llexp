#pragma once

#include <queue>
#include <mutex>
#include <cstring>
#include "common.h"

struct QueueWrapper {
public:
 std::queue<struct timespec> queue;
 std::mutex queue_mutex;

 static const size_t MAX_SAMPLES = 1000000U;

 struct timespec write_v1() 
 {
    std::unique_lock<std::mutex> guard( queue_mutex );
    struct timespec write_time;
    clock_gettime( CLOCK_MONOTONIC, &write_time );
    queue.push( write_time );
    return write_time;
 }

 TimespecPair * read_v1( TimespecPair * next_sample ) 
 {
   std::unique_lock<std::mutex> guard( queue_mutex );
   if( !queue.empty()) {
     struct timespec write_time = queue.front();
     queue.pop();

     struct timespec read_time;
     clock_gettime( CLOCK_MONOTONIC, &read_time );

     next_sample->write_time = write_time;
     next_sample->read_time = read_time;
     next_sample++;
   }
   return next_sample;
 } 

 struct timespec write_v2() 
 {
    queue_mutex.lock();

    struct timespec write_time;
    clock_gettime( CLOCK_MONOTONIC, &write_time );
    queue.push( write_time );
    queue_mutex.unlock();
    return write_time;
 }

 TimespecPair * read_v2( TimespecPair * next_sample ) 
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

 //

 struct timespec write() 
 {
     return write_v2();
 } 

 TimespecPair * read( TimespecPair * next_sample ) 
 {
     return read_v2( next_sample );
 }
};

