#pragma once

#include <queue>
#include <mutex>
#include <cstring>
#include "common.h"
#include <pthread.h>
#include <memory>
#include <boost/thread/thread.hpp>
#include <boost/lockfree/spsc_queue.hpp>
//#include <boost/atomic.hpp>

struct QueueWrapper {
public:

  boost::lockfree::spsc_queue<TimespecPair64, boost::lockfree::capacity<65000>> queue0;
  boost::lockfree::spsc_queue<TimespecPair64, boost::lockfree::capacity<65000>> queue1;

  const TimespecPair64 null_msg{{0,0},{0,0},""};
 
  static const size_t MAX_SAMPLES = 1000000U;
 
  QueueWrapper() {
  }
//  ~QueueWrapper() {
//  }
 
  TimespecPair64 write_blocking(int queue_id) 
  {
     TimespecPair64 msg;
     clock_gettime( USE_CLOCK_TYPE, &msg.write_time );
     if( queue_id == 1 ) {
         bool success = queue1.push( msg );
         while( !success ) {
             success = queue1.push( msg );
         } 
     } else {
         bool success = queue0.push( msg );
         while( !success ) {
             success = queue0.push( msg );
         } 
     }
     return msg;
  }
 
  TimespecPair64 * read0( TimespecPair64 * next_sample ) 
  {
      if( !queue0.empty()) {
        if( queue0.pop( *next_sample )) 
        {
            clock_gettime( USE_CLOCK_TYPE, &( next_sample->read_time ));
            next_sample++;
        }
      }
      return next_sample;
  }

  TimespecPair64 * read1( TimespecPair64 * next_sample ) 
  {
      if( !queue1.empty()) {
        if( queue1.pop( *next_sample )) 
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

    //TimespecPair64 msg0;
    // TimespecPair64 msg1;

    while(( !queue0.empty() || !queue1.empty())  && ( num_drained < max_num_drain )) 
    {
        if( !queue0.empty()) {
            if( queue0.pop( *next_sample++ )) {
                //*next_sample++ = msg0;
                num_drained++;
            }
        }
        if( !queue1.empty()) {
            if( queue1.pop( *next_sample++ )) {
                //*next_sample++ = msg1;
                num_drained++;
            }
        }
        // todo stream sort
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

