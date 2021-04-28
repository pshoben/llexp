#pragma once

#include <time.h>
#include <stdint.h>
#include <x86intrin.h>

#define USE_CLOCK_TYPE CLOCK_REALTIME

typedef struct timespec Timespec;

typedef struct TimespecPair {
  Timespec write_time; // = {0,0};
  Timespec read_time; // = {0,0};
} TimespecPair;

typedef struct TimespecPair64 {
  Timespec write_time; // = {0,0};
  Timespec read_time; // = {0,0};
  char padding[64-(sizeof(Timespec)*2)];
} TimespecPair64;


inline long timespec_diff_ns( struct timespec start_time, struct timespec end_time )
{
  // return total difference in nanoseconds between start time and end time (combining the seconds and ns members into a single value)
  // expects end time > start time
  // will overflow if the difference is too large to be expressed in the available range
  //const time_t max_seconds = (time_t) INT32_MAX;

  time_t diff_secs = end_time.tv_sec - start_time.tv_sec;
  if( diff_secs > 0 ) {
      return (long)(diff_secs * 1000000000L) + end_time.tv_nsec - start_time.tv_nsec;
  } else {
      return end_time.tv_nsec - start_time.tv_nsec;
  }
}

inline void spin_for_nanos( long wait_time ) 
{
    struct timespec start_time;
    struct timespec end_time;
//std::cout << "waiting for " << wait_time << "\n";
    clock_gettime( USE_CLOCK_TYPE, &start_time );

    long diff = 0;
    do {
       clock_gettime( USE_CLOCK_TYPE, &end_time );
   
       diff =  timespec_diff_ns( start_time,  end_time );
    } while ( diff < wait_time ); 
}
 

inline double calculate_throughput_mb(unsigned int msg_per_sec, size_t msg_size ) 
{
  return (double)((double)msg_size * (double)msg_per_sec / 1048576.0);
}

inline unsigned int calculate_throughput_msgs( double mb_per_sec, size_t msg_size ) 
{
   return (unsigned int) (double)(1048576.0 * mb_per_sec / (double) msg_size);
}


inline double measure_nanos_per_cycle( ) 
{

    struct timespec start_time;
    clock_gettime( CLOCK_REALTIME, &start_time );

    uint64_t start_cycles = __rdtsc();
    uint64_t end_cycles;

    int countdown = 100000000;
    while( countdown--> 0 )
    {
        end_cycles = __rdtsc();
    }
    uint64_t diff_cycles = end_cycles - start_cycles;

    struct timespec end_time;
    clock_gettime( CLOCK_REALTIME, &end_time );
    long diff_nanos =  timespec_diff_ns( start_time,  end_time );

    double nanos_per_cycle = (double) diff_nanos / (double) diff_cycles;

    return nanos_per_cycle; 
}

 
