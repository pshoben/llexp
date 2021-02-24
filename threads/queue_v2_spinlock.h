#pragma once

#include <queue>
#include <mutex>
#include <cstring>
#include "common.h"
#include <pthread.h>
#include <memory>

struct QueueWrapper {
public:
  std::queue<struct timespec> queue;
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
     clock_gettime( CLOCK_REALTIME, &write_time );
 
     int success = pthread_spin_lock( &spinlock );
     if( success != 0 ) {
        // TODO
        // EDEADLK - the lock is already held by this thread
        // EINVAL - lock not initialised
        return null_time;
 
     } else {    
 
       queue.push( write_time );
   
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
         queue.pop();
  
         success = pthread_spin_unlock( &spinlock );
         if( success != 0 ) {
           // TODO
           // EPERM - the thread does not hold  the lock
           // EINVAL - lock not initialised
         }
   
         clock_gettime( CLOCK_REALTIME, &( next_sample->read_time ));
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
};

