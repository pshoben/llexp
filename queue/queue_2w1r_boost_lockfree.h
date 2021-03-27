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
  boost::lockfree::queue<TimespecPair64, boost::lockfree::capacity<65000>> queue;
  const TimespecPair64 null_msg{{0,0},{0,0},""};
 
  static const size_t MAX_SAMPLES = 1000000U;
 
  QueueWrapper() {
  }
//  ~QueueWrapper() {
//  }
 
  TimespecPair64 write_blocking() 
  {
     TimespecPair64 msg;
     clock_gettime( USE_CLOCK_TYPE, &msg.write_time );
     bool success = queue.push( msg );
     while( !success ) {
         success = queue.push( msg );
     } 
     return msg;
  }
 
  TimespecPair64 * read( TimespecPair64 * next_sample ) 
  {
      if( !queue.empty()) {
        if( queue.pop( *next_sample )) 
        {
            clock_gettime( USE_CLOCK_TYPE, &( next_sample->read_time ));
  
            next_sample++;
        }
      }
      return next_sample;
  }
 
  unsigned int drain( TimespecPair64 * next_sample, unsigned int max_num_drain ) 
  {
    unsigned int num_drained = 0;
    TimespecPair64 * first_sample = next_sample;

    //struct timespec val;
    while( !queue.empty() && ( num_drained < max_num_drain )) 
    {
      if( queue.pop( *next_sample )) {
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

