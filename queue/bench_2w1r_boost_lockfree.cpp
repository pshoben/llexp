#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <cstdio>
#include <unistd.h>
#include <stdint.h>
#include "common.h"
#include <algorithm>
#include "message_stats.h"
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include "queue_2w1r_boost_lockfree.h"

using std::vector;
using std::mutex;

size_t g_msg_size = sizeof(TimespecPair64);
unsigned int g_num_writers=2;
unsigned int g_max_samples=QueueWrapper::MAX_SAMPLES;
unsigned int g_msg_per_sec=100000;
double g_mb_per_sec = calculate_throughput_mb( g_msg_per_sec, g_msg_size );
unsigned int g_max_writes;

bool g_verbose = false;
bool g_reader_thread_ready = false;

MessageStats * g_stats = nullptr;
QueueWrapper g_queue{};
std::mutex g_mutex;

void take_mutex() { 
  g_mutex.lock(); 
}
void release_mutex() {
  g_mutex.unlock();
}

TimespecPair64 * create_samples( unsigned int max_reads )
{
  TimespecPair64 * samples;
  samples = new TimespecPair64[ max_reads ] ;
  std::memset( samples, 0, sizeof( TimespecPair64 ) * max_reads ) ;
  return samples;
}

void * reader_thread_func( __attribute__((unused)) void * args ) 
{
    take_mutex();
    unsigned int max_reads = g_max_samples - g_num_writers; 

    TimespecPair64 * samples = create_samples( max_reads );
    g_stats->add_batch( samples ) ;
    TimespecPair64 * next_sample = samples;

    if( g_verbose )
    {
        pid_t tid = syscall( __NR_gettid );
        std::cout << "Reader Thread ID " << tid << " on CPU " << sched_getcpu() << " - reading " << max_reads << " msgs\n";
    }

    release_mutex();

    g_reader_thread_ready = true;

    unsigned int num_reads = 0;

    while( true ) {
 
      TimespecPair64 * current_sample = next_sample;
 
      next_sample = g_queue.read( next_sample ) ; 
 
      if( next_sample != current_sample && ( !( next_sample == nullptr )))
      {
         num_reads ++;
      }
 
      // check if all msgs received so exit thread:
      if( num_reads >= max_reads ) 
      {
        if( g_verbose )
        {
           take_mutex();
           std::cout << "Read Thread on CPU " << sched_getcpu() << " - done \n";
           release_mutex();
        }
        break;
      }
      // no delay - readers run hot
     }
 
     {
         take_mutex();
         if( g_verbose ) {
           std::cout << "Read Thread on CPU " << sched_getcpu() << " - read " << num_reads << " msgs \n";
         }
         release_mutex();
     }
     return nullptr;
}

void * writer_thread_func( __attribute__((unused)) void * args ) 
{
    take_mutex();
    release_mutex();

    long target_delay = ((long)(double)(1000000000.0 / g_msg_per_sec)) ;

    if( g_verbose )
    {
        take_mutex();
        pid_t tid = syscall( __NR_gettid );
        std::cout << "Writer Thread ID " << tid << " on CPU " << sched_getcpu() << " - writing " << g_max_writes << " msgs at rate " << g_msg_per_sec << " msg/sec with target delay " << target_delay << " ns per msg\n";
        release_mutex();
    }
    unsigned int count = 0;  
    long adjusted_wait_time = 0;
    struct timespec prev_write_time = {0,0}; 

    while( count < g_max_writes ) 
    {
        TimespecPair64 msg = g_queue.write_blocking() ;
        count++;

        if( prev_write_time.tv_sec != 0 ) {

            long write_delay = timespec_diff_ns( prev_write_time, msg.write_time ) ;
            adjusted_wait_time += target_delay - write_delay ;
        } 

        if( adjusted_wait_time > 0 ) {
          spin_for_nanos( adjusted_wait_time ); 
          //std::this_thread::sleep_for( std::chrono::nanoseconds( adjusted_wait_time ));
        }

       prev_write_time = msg.write_time;
    }

    if( g_verbose )
    {
         take_mutex();
         std::cout << "Thread on CPU " << sched_getcpu() << " finished after " << count << " write ops " << "\n";
         release_mutex();
    }
    return nullptr;
}

int main(int argc, char * const * argv) 
{
  char c;
  while ((c = getopt (argc, argv, "vm:r:n:r:t:")) != -1) {
    switch (c)
    {
      case 'v':
        g_verbose = true;
        break;
      case 'n':
      {
        g_num_writers = std::max(1,atoi(optarg)/2);
        break;
      }
      case 'm':
      {
        unsigned int max_samples = (unsigned int) atoi(optarg);
        if ( max_samples <= QueueWrapper::MAX_SAMPLES )  {
           g_max_samples = max_samples;
        }
        if( g_verbose ) {
            std::cout << "got -m num messages " << g_max_samples << "\n";
        }
        break;
      }
      case 'r':
      {
        g_msg_per_sec = (unsigned int) atoi(optarg);
        g_mb_per_sec = calculate_throughput_mb( g_msg_per_sec, g_msg_size );
        g_max_samples = 40 * g_msg_per_sec; // 2 writers = 20 seconds
        if( g_verbose ) {
            std::cout << "got -r rate " << g_msg_per_sec << " - setting max messages " << g_max_samples << "\n";
        }
        break;
      }
      case 't':
      {
        g_mb_per_sec = atof(optarg);
        g_msg_per_sec = calculate_throughput_msgs( g_mb_per_sec, g_msg_size );
        break;
      }
      case '?':
      default:
        printf("usage: (-v (=verbose)) (-m num-messages) (-r rate-msg-per-sec) (-t throughput-MB-per-sec) -n num-threads\n");
        exit(0);
     }
  }

  if( g_verbose ) {
    std::cout << "queue " << (g_queue.queue.is_lock_free() ? " IS ": " IS NOT ") << "lockfree\n";
    std::cout << "msg size " << sizeof(TimespecPair64) << " bytes\n";
    std::cout << "target throughput " << (sizeof(TimespecPair64)*g_num_writers*g_msg_per_sec)/1024 << " MB/sec\n";

  }

  g_max_writes = g_max_samples / g_num_writers; 

  g_stats = new MessageStats{ "lockfree ", g_max_writes, (unsigned int) g_msg_per_sec, g_num_writers };

  pthread_t reader_threads[ 1 ];
  pthread_t writer_threads[ g_num_writers ];

  pthread_attr_t attr;
  pthread_attr_init( &attr );

  cpu_set_t cpuset;

  // create reader at cpu 1:
  CPU_ZERO( &cpuset );
  CPU_SET( 1, &cpuset );

  pthread_attr_setaffinity_np( &attr, sizeof( cpu_set_t ), &cpuset );
  pthread_create( &reader_threads[0], &attr, reader_thread_func, NULL );

   // don't start writers until setup is complete
  while( ! g_reader_thread_ready ) {
      std::this_thread::sleep_for( std::chrono::milliseconds( 100 ));
  }
 
  for( unsigned int i = 0 ; i < g_num_writers ; i++ ) 
  {
      // create writer at next cpu:
      CPU_ZERO( &cpuset );
      CPU_SET( 2 + i , &cpuset );

      pthread_attr_setaffinity_np( &attr, sizeof( cpu_set_t ), &cpuset );
      pthread_create( &writer_threads[i], &attr, writer_thread_func, NULL );
  }

  for( unsigned int i = 0 ; i < g_num_writers ; i++ ) 
  { 
      pthread_join( writer_threads[i], NULL );
  }
  pthread_join( reader_threads[0], NULL );

  g_stats->print_stats();

  exit(0);
}
