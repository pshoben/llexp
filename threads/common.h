#pragma once

#include <time.h>
#include <stdint.h>

typedef struct timespec Timespec;

typedef struct TimespecPair {
  Timespec write_time; // = {0,0};
  Timespec read_time; // = {0,0};
} TimespecPair;

  inline long timespec_diff_ns(struct timespec start_time, struct timespec end_time )
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


inline double calculate_throughput_mb(unsigned int msg_per_sec, size_t msg_size ) 
{
  return (double)((double)msg_size * (double)msg_per_sec / 1048576.0);
}
inline unsigned int calculate_throughput_msgs( double mb_per_sec, size_t msg_size ) 
{
   return (unsigned int) (double)(1048576.0 * mb_per_sec / (double) msg_size);
} 
