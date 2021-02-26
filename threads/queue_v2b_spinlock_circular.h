#pragma once

#include <queue>
#include <mutex>
#include <cstring>
#include "common.h"
#include <pthread.h>
#include <memory>
#include <boost/circular_buffer.hpp>

struct QueueWrapper {
public:
  boost::circular_buffer<struct timespec> queue{ 1000U };
  pthread_spinlock_t spinlock;
  const struct timespec null_time{0,0};
 
  static const size_t MAX_SAMPLES = 1000000U;
 
  QueueWrapper() {
     int success = pthread_spin_init( &spinlock, PTHREAD_PROCESS_SHARED );
     if( success != 0 ) {
        // TODO
        // EAGAIN - no resources
        // EBUSY - lock already in use
        // EINVAL - invalid lock
     } 
  }
  ~QueueWrapper() {
     int success = pthread_spin_destroy( &spinlock );
     if( success != 0 ) {
        // TODO
        // EAGAIN - no resources
        // EBUSY - lock already in use
        // EINVAL - invalid lock
     } 
  }
 
 
 
  struct timespec write() 
  {
     struct timespec write_time;
     clock_gettime( USE_CLOCK_TYPE, &write_time );
 
     int success = pthread_spin_lock( &spinlock );
     if( success != 0 ) {
        // TODO
        // EDEADLK - the lock is already held by this thread
        // EINVAL - lock not initialised
        return null_time;
 
     } else {    
 
       queue.push_back( write_time );
   
       success = pthread_spin_unlock( &spinlock );
       if( success != 0 ) {
         // TODO
         // EPERM - the thread does not hold  the lock
         // EINVAL - lock not initialised
         return null_time;
       }
       return write_time;
     }
  }
 
  TimespecPair * read( TimespecPair * next_sample ) 
  {
    int success = pthread_spin_lock( &spinlock );
    if( success != 0 ) {
  
       // TODO
       // EDEADLK - the lock is already held by this thread
       // EINVAL - lock not initialised
       return nullptr;
 
    } else {    
 
      if( !queue.empty()) {
 
         next_sample->write_time = queue.front();
         queue.pop_front();
  
         success = pthread_spin_unlock( &spinlock );
         if( success != 0 ) {
           // TODO
           // EPERM - the thread does not hold  the lock
           // EINVAL - lock not initialised
         }
   
         clock_gettime( USE_CLOCK_TYPE, &( next_sample->read_time ));
         next_sample++;
  
      } else {
  
         // queue is empty, dont advance
  
         success = pthread_spin_unlock( &spinlock );
         if( success != 0 ) {
           // TODO
           // EPERM - the thread does not hold  the lock
           // EINVAL - lock not initialised
         }
      }
      return next_sample;
    }
  }

 
  unsigned int drain( TimespecPair * next_sample, unsigned int max_num_drain ) 
  {
    int success = pthread_spin_lock( &spinlock );
    if( success != 0 ) {
       // TODO
       // EDEADLK - the lock is already held by this thread
       // EINVAL - lock not initialised
       return 0;
    }

    unsigned int num_drained = 0;
    TimespecPair * first_sample = next_sample;

    while ( !queue.empty() && num_drained < max_num_drain ) {
 
      next_sample->write_time = queue.front();
      queue.pop_front();
 
      next_sample++;
      num_drained++;
    }

    success = pthread_spin_unlock( &spinlock );
    if( success != 0 ) {
        // TODO
        // EPERM - the thread does not hold  the lock
        // EINVAL - lock not initialised
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

