#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <cstdio>
#include <unistd.h>
#include "queue_wrapper.h"
#include <stdint.h>
#include "common.h"
#include <algorithm>
//#include <atomic>
#include "message_stats.h"

using std::vector;
using std::mutex;

size_t g_msg_size = sizeof(TimespecPair);
unsigned int g_num_thread_pairs=2;
unsigned int g_max_samples=1000000;
unsigned int g_msg_per_sec=100;
double g_mb_per_sec = calculate_throughput_mb( g_msg_per_sec, g_msg_size );

bool g_verbose = false;

void start_threads( unsigned int num_threads, vector<std::thread> & threads, const char * msg, int is_reader )
{
  QueueWrapper queue{};
  mutex iomutex;
  unsigned int max_writes = g_max_samples / g_num_thread_pairs; 
  bool verbose = g_verbose;
  MessageStats stats{ g_num_thread_pairs, max_writes, (unsigned int) g_msg_per_sec, g_num_thread_pairs };

  for (unsigned i = 0; i < num_threads; ++i) {

    if( is_reader ) 
    {
       // reader thread lambda
       threads[i] = std::thread([ verbose, &iomutex, i, msg, &queue, max_writes, &stats ] {

       TimespecPair * samples = new TimespecPair[ max_writes ] ;
       std::memset( samples, 0, sizeof( TimespecPair ) * max_writes ) ;

       TimespecPair * next_sample = samples;
       if( verbose )
       {
          std::lock_guard<mutex> iolock(iomutex);
          std::cout << "Thread #" << i << ": on CPU " << sched_getcpu() << " - " << msg << " waiting for " << max_writes << " msgs\n";
       }

       unsigned int num_reads = 0;
       while( true ) {

         TimespecPair * current_sample = next_sample;
         next_sample = queue.read( next_sample ) ; 
         if( next_sample != current_sample )
         {
            num_reads ++;

            if( verbose ) 
            {
              std::lock_guard<mutex> iolock(iomutex);
              std::cout << "Reader Thread #" << i << ": on CPU " << sched_getcpu()
              << " num reads = " << num_reads  
              << " write time = " << current_sample->write_time.tv_sec << "." << current_sample->write_time.tv_nsec
              << " read time = " << current_sample->read_time.tv_sec << "." << current_sample->read_time.tv_nsec
              << " latency = " << timespec_diff_ns( current_sample->write_time, current_sample->read_time )  << "\n";
            }
         }
         // check if all msgs received so exit thread:
         if( num_reads >= max_writes ) {
           if( verbose )
           {
              std::lock_guard<mutex> iolock(iomutex);
              std::cout << "Read Thread #" << i << ": on CPU " << sched_getcpu() << " - done \n";
           }
           break;
         }
         // no delay - readers run "hot"
        }
        {
            std::lock_guard<mutex> iolock(iomutex);
            if( verbose ) {
              std::cout << "Read Thread #" << i << ": on CPU " << sched_getcpu() << " - read " << num_reads << " msgs \n";
            }
            // end reader : submit results to stats engine
            stats.add_batch( samples ) ;
        }
      });
    } else {

      double msg_per_sec = (double) g_msg_per_sec;
      long target_delay = ((long)(double)(1000000000.0 / msg_per_sec)) ;
      // writer thread lambda
      threads[i] = std::thread([ verbose, &iomutex, i, msg, &queue, max_writes, msg_per_sec, target_delay ] {

          if( verbose )
          {
              std::lock_guard<mutex> iolock(iomutex);
              std::cout << "Thread #" << i << ": on CPU " << sched_getcpu() << " - " << msg << " writing " << max_writes << " msgs at rate " << msg_per_sec << " msg/sec with target delay " << target_delay << " ns per msg\n";
          }
          unsigned int count = 0;  
          long adjusted_wait_time = target_delay;
          struct timespec prev_write_time = {0,0}; 
          while( count < max_writes ) 
          {
              struct timespec write_time = queue.write() ;
              count++;

              if( prev_write_time.tv_sec != 0 ) {
                  long write_delay = timespec_diff_ns( prev_write_time, write_time ) ;
                  adjusted_wait_time = target_delay - write_delay ;
//          {
//              std::lock_guard<mutex> iolock(iomutex);
//              std::cout << "Thread #" << i << ": on CPU " << sched_getcpu() 
//              << " target delay " << target_delay << " write delay " << write_delay << " adjusted wait time " << adjusted_wait_time << "\n";
//          }
 
              } 
              if( adjusted_wait_time > 0 ) { 
                // Simulate important work done by the thread by sleeping for a bit...
                std::this_thread::sleep_for( std::chrono::nanoseconds( adjusted_wait_time ));
              }

             prev_write_time = write_time;
          }
          if( verbose )
          {
             std::lock_guard<mutex> iolock(iomutex);
             std::cout << "Thread #" << i << ": on CPU " << sched_getcpu() << " finished after " << count << " write ops " << "\n";
          }
      });
    }
    // Create a cpu_set_t object representing a set of CPUs. Clear it and mark
    // only CPU i as set.

    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(i*2+is_reader, &cpuset);
    int rc = pthread_setaffinity_np(threads[i].native_handle(),
                                    sizeof(cpu_set_t), &cpuset);
//    if (rc != 0) 
//    {
//      std::cerr << "Error calling pthread_setaffinity_np: " << rc << "\n";
//    }
  }
}

int main(int argc, char * const * argv) 
{
  char c;
  while ((c = getopt (argc, argv, "vm:r:n:r:t:")) != -1) {
    switch (c)
    {
      case 'v':
        g_verbose = true;
//        printf("got -v verbose mode \n"); 
        break;
      case 'n':
      {
        g_num_thread_pairs = std::max(1,atoi(optarg)/2);
//        printf("got -n num threads = %d thread pairs\n", g_num_thread_pairs); 
        break;
      }
      case 'm':
      {
        unsigned int max_samples = (unsigned int) atoi(optarg);
        if ( max_samples < g_max_samples )  {
           g_max_samples = max_samples;
        }
//        printf("got -m num messages = %u , using value %u (max %u)\n", max_samples,  g_max_samples, 1000000); 
        break;
      }
      case 'r':
      {
        g_msg_per_sec = (unsigned int) atoi(optarg);
        g_mb_per_sec = calculate_throughput_mb( g_msg_per_sec, g_msg_size );
//        printf("got -r rate = %u messages per sec ( = %0.3f MBytes/sec using msg size = %lu bytes)\n", g_msg_per_sec, g_mb_per_sec, g_msg_size ); 
        break;
      }
      case 't':
      {
        g_mb_per_sec = atof(optarg);
        g_msg_per_sec = calculate_throughput_msgs( g_mb_per_sec, g_msg_size );
//        printf("got -t throughput = %u messages per sec ( = %0.3f MBytes/sec using msg size = %lu bytes)\n", g_msg_per_sec, g_mb_per_sec, g_msg_size ); 
        break;
      }
 
      case '?':
      default:
        printf("usage: (-v (=verbose)) (-m num-messages) (-r rate-msg-per-sec) (-t throughput-MB-per-sec) -n num-threads\n");
        exit(0);
     }
  }
//  printf("usage: (-v (=verbose)) (-m num-messages) (-r rate-msg-per-sec) (-t throughput-MB-per-sec)  -n num-threads\n");
//  printf("running with %d thread pairs (%d threads)\n", g_num_thread_pairs, g_num_thread_pairs * 2); 

  vector<std::thread> reader_threads( g_num_thread_pairs );
  vector<std::thread> writer_threads( g_num_thread_pairs );

  start_threads( g_num_thread_pairs, reader_threads, "Reader " , 1 );
  start_threads( g_num_thread_pairs, writer_threads, "Writer " , 0 );

  for (auto& t : reader_threads) {
    t.join();
  }
  for (auto& t : writer_threads) {
    t.join();
  }
}

