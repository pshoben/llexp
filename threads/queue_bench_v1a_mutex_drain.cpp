#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <cstdio>
#include <unistd.h>
#include "queue_v1_mutex.h"
#include <stdint.h>
#include "common.h"
#include <algorithm>
#include "message_stats.h"
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <algorithm>

using std::vector;
using std::mutex;

size_t g_msg_size = sizeof(TimespecPair);
unsigned int g_num_thread_pairs=2;
unsigned int g_max_samples=1000000;
unsigned int g_msg_per_sec=100;
double g_mb_per_sec = calculate_throughput_mb( g_msg_per_sec, g_msg_size );
unsigned int g_max_writes;

bool g_verbose = false;

MessageStats * g_stats = nullptr;
QueueWrapper g_queue{};
std::mutex g_mutex;

void take_mutex() { 
  g_mutex.lock(); 
}
void release_mutex() {
  g_mutex.unlock();
}

TimespecPair * create_samples( unsigned int max_writes )
{
  TimespecPair * samples;
  samples = new TimespecPair[ max_writes ] ;
  std::memset( samples, 0, sizeof( TimespecPair ) * max_writes ) ;
  return samples;
}

void * reader_thread_func( __attribute__((unused)) void * args ) 
{
    take_mutex();
 
    TimespecPair * samples = create_samples( g_max_writes );
    g_stats->add_batch( samples ) ;
    TimespecPair * next_sample = samples;
 
    if( g_verbose )
    {
        pid_t tid = syscall( __NR_gettid );
        std::cout << "Reader Thread ID " << tid << " on CPU " << sched_getcpu() << " - reading " << g_max_writes << " msgs\n";
    }

    release_mutex();
 
    unsigned int num_reads = 0;

    while( true ) {
 
      //TimespecPair * current_sample = next_sample;

      unsigned int num_drained = g_queue.drain( next_sample, std::min( 100U, g_max_writes - num_reads )) ; 
 
      if( num_drained > 0 ) 
      {
         next_sample += num_drained;
         num_reads += num_drained;
 
 ////            if( verbose ) 
 ////            {
 //              unsigned int latency = timespec_diff_ns( current_sample->write_time, current_sample->read_time );
 //              if( latency > 1000000 ) {
 //                std::lock_guard<mutex> g_mutex(queue.g_mutex);
 //                std::cout << "Reader Thread #" << i << ": on CPU " << sched_getcpu()
 //                << " num reads = " << num_reads  
 //                << " write time = " << current_sample->write_time.tv_sec << "." << current_sample->write_time.tv_nsec
 //                << " read time = " << current_sample->read_time.tv_sec << "." << current_sample->read_time.tv_nsec
 //                << " latency = " << timespec_diff_ns( current_sample->write_time, current_sample->read_time )  << "\n";
 //              }
 ////            }
      }
 
      // check if all msgs received so exit thread:
      if( num_reads >= g_max_writes ) 
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
        struct timespec write_time = g_queue.write() ;
        count++;

        if( prev_write_time.tv_sec != 0 ) {

            long write_delay = timespec_diff_ns( prev_write_time, write_time ) ;
            adjusted_wait_time += target_delay - write_delay ;
        } 

        if( adjusted_wait_time > 0 ) {
          spin_for_nanos( adjusted_wait_time ); 
          //std::this_thread::sleep_for( std::chrono::nanoseconds( adjusted_wait_time ));
        }

       prev_write_time = write_time;
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
        g_num_thread_pairs = std::max(1,atoi(optarg)/2);
        break;
      }
      case 'm':
      {
        unsigned int max_samples = (unsigned int) atoi(optarg);
        if ( max_samples < g_max_samples )  {
           g_max_samples = max_samples;
        }
        break;
      }
      case 'r':
      {
        g_msg_per_sec = (unsigned int) atoi(optarg);
        g_mb_per_sec = calculate_throughput_mb( g_msg_per_sec, g_msg_size );
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

  g_max_writes = g_max_samples / g_num_thread_pairs; 

  g_stats = new MessageStats{ "mut+drain", g_max_writes, (unsigned int) g_msg_per_sec, g_num_thread_pairs };

  pthread_t reader_threads[ g_num_thread_pairs ];
  pthread_t writer_threads[ g_num_thread_pairs ];

  pthread_attr_t attr;
  pthread_attr_init( &attr );

  cpu_set_t cpuset;

  for( unsigned int i = 0 ; i < g_num_thread_pairs ; i++ ) 
  {
      // create writer at odd numbered cpu:
      CPU_ZERO( &cpuset );
      CPU_SET( i*2 + 1, &cpuset );

      pthread_attr_setaffinity_np( &attr, sizeof( cpu_set_t ), &cpuset );
      pthread_create( &writer_threads[i], &attr, writer_thread_func, NULL );

      // create reader at even numbered cpu:
      CPU_ZERO( &cpuset );
      CPU_SET( i*2 + 2, &cpuset );

      pthread_attr_setaffinity_np( &attr, sizeof( cpu_set_t ), &cpuset );
      pthread_create( &reader_threads[i], &attr, reader_thread_func, NULL );
  }

  for( unsigned int i = 0 ; i < g_num_thread_pairs ; i++ ) 
  { 
    pthread_join( writer_threads[i], NULL );
    pthread_join( reader_threads[i], NULL );
  }

  g_stats->print_stats();

  exit(0);
}
