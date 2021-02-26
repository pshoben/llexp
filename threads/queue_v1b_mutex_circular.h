#pragma once

#include <queue>
#include <mutex>
#include <cstring>
#include "common.h"
#include <boost/circular_buffer.hpp>

//typedef struct timespec message_type ;

struct QueueWrapper {
public:
 boost::circular_buffer<struct timespec> queue{ 1000U };
 std::mutex queue_mutex;
 const struct timespec null_time{0,0};
 
 static const size_t MAX_SAMPLES = 1000000U;


















 struct timespec write() 
 {
    struct timespec write_time;
    clock_gettime( USE_CLOCK_TYPE, &write_time );
    queue_mutex.lock();
    queue.push_back( write_time );
    queue_mutex.unlock();
    return write_time;
 }

 TimespecPair * read( TimespecPair * next_sample ) 
 {
   queue_mutex.lock();
   if( !queue.empty()) {

     next_sample->write_time = queue.front();
     queue.pop_front();
     queue_mutex.unlock();

     // time copied to local memory - after lock released
     clock_gettime( USE_CLOCK_TYPE, &next_sample->read_time );
     next_sample++;

   } else {
     queue_mutex.unlock();
   }
   return next_sample;
 } 

 unsigned int drain( TimespecPair * next_sample, unsigned int max_num_drain ) 
 {
   queue_mutex.lock();

   unsigned int num_drained = 0;
   TimespecPair * first_sample = next_sample;

   while ( !queue.empty() && num_drained < max_num_drain ) {

     next_sample->write_time = queue.front();
     queue.pop_front();

     next_sample++;
     num_drained++;
   }

   queue_mutex.unlock();

   // time copied to local memory and after lock released

   struct timespec read_time;
   clock_gettime( USE_CLOCK_TYPE, &read_time );

   while( first_sample < next_sample ) {
     first_sample->read_time = read_time;
     first_sample++;
   }

   return num_drained;
 } 

};

