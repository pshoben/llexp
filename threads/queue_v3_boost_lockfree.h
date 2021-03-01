#pragma once

#include <queue>
#include <mutex>
#include <cstring>
#include "common.h"
#include <pthread.h>
#include <memory>
#include <boost/thread/thread.hpp>
#include <boost/lockfree/queue.hpp>
//#include <boost/atomic.hpp>

struct QueueWrapper {
public:
  boost::lockfree::queue<struct timespec, boost::lockfree::capacity<65000>> queue;
  const struct timespec null_time{0,0};
 
  static const size_t MAX_SAMPLES = 1000000U;
 
  QueueWrapper() {
  }
//  ~QueueWrapper() {
//  }
 
  struct timespec write() 
  {
     struct timespec write_time;
     clock_gettime( USE_CLOCK_TYPE, &write_time );
// std::cout << "write time " << write_time.tv_nsec << "\n"; // TODO REMOVE

     if( queue.push( write_time )) {
         return write_time;
     } else {
         return null_time;
     }
  }
 
  TimespecPair * read( TimespecPair * next_sample ) 
  {
      if( !queue.empty()) {
        if( queue.pop( next_sample->write_time )) 
        {
//  std::cout << "read got time " << next_sample->write_time.tv_nsec << "\n"; // TODO REMOVE
            clock_gettime( USE_CLOCK_TYPE, &( next_sample->read_time ));
//   std::cout << "read time " << next_sample->read_time.tv_nsec << "\n"; // TODO REMOVE
//  std::cout << "latency  " << next_sample->read_time.tv_nsec - next_sample->write_time.tv_nsec <<"\n"; 
  
            next_sample++;
        }
      }
      return next_sample;
  }
 
  unsigned int drain( TimespecPair * next_sample, unsigned int max_num_drain ) 
  {
    unsigned int num_drained = 0;
    TimespecPair * first_sample = next_sample;

    //struct timespec val;
    while( !queue.empty() && ( num_drained < max_num_drain )) 
    {
      if( queue.pop( next_sample->write_time )) {
        //next_sample->write_time = val;
        next_sample++;
        num_drained++;
      }
    }

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

