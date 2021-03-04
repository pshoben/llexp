#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
//#include <cstdio>
#include <unistd.h>
//#include "queue_v2_spinlock.h"
#include <stdint.h>
//#include "common.h"
//#include <algorithm>
//#include "message_stats.h"
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
//#include <algorithm>
#include <atomic>
#include <cstdlib>
#include <x86intrin.h>


using std::vector;
using std::mutex;

unsigned int g_num_threads=8;
uint64_t g_max_reads=1000000000;

bool g_verbose = true;

pthread_t threads[ 8 ];

std::mutex g_mutex;

std::atomic<bool> g_running = true;
std::atomic<unsigned int> g_num_threads_finished = 0;

void take_mutex() { 
  g_mutex.lock(); 
}
void release_mutex() {
  g_mutex.unlock();
}

void * thread_func( __attribute__((unused)) void * args ) 
{
#ifdef USE_RDTSC
    auto option = "rdtsc";
#endif
 
#ifdef USE_RDTSCP
    auto option = "rdtscp";
#endif


    take_mutex();
    if( g_verbose )
    {
        pid_t tid = syscall( __NR_gettid );
        std::cout << "Thread ID " << tid << " on CPU " << sched_getcpu() << " - reading " << g_max_reads << " samples\n";
    }
    release_mutex();
 
    uint64_t num_reads = 0;
    unsigned int aux;

#ifdef USE_RDTSC
    auto prev = __rdtsc();
#endif
 
#ifdef USE_RDTSCP
    auto prev = __rdtscp( &aux );
#endif

    uint64_t sum_diff = 0; 
    uint64_t sum_mean_diff = 0; 

    uint64_t running_avg = 0;
    int avg_countdown = 1000000;

    while( g_running && num_reads < g_max_reads ) 
    {

#ifdef USE_RDTSC
        auto time = __rdtsc();
#endif
 
#ifdef USE_RDTSCP
        auto time = __rdtscp( &aux );
#endif
        auto diff = time - prev;
        sum_diff += diff;
        if( running_avg > 0) {
            sum_mean_diff += llabs(diff - running_avg);
        }
        avg_countdown--;

        // every so often, get the average (so far)
        if( avg_countdown == 0 ) {

            running_avg = sum_diff / num_reads;
            avg_countdown = 1000000;

//            if( g_verbose )
//            {
//                take_mutex();
//                std::cout << "| option " << option << " | cpu " << sched_getcpu() << " | count " << num_reads << " | avg " << (sum_diff / num_reads) << " | mad " << (sum_mean_diff / num_reads) << "\n";
//                release_mutex();
//            }
        } 
        prev = time;
        num_reads ++;
 
        // no delay - run hot
    }
 
    take_mutex();
    if( g_verbose ) {
        std::cout << "Thread on CPU " << sched_getcpu() << " - read " << num_reads << " samples \n";
    }
    std::cout << "| option " << option << " | cpu " << sched_getcpu() << " | count " << num_reads << " | avg " << (sum_diff / num_reads) << " | mad " << (sum_mean_diff / num_reads) << "\n";
    release_mutex();

    g_num_threads_finished++;
    return nullptr;
}

int main() //  int argc, char * const * argv ) 
{
  pthread_attr_t attr;
  pthread_attr_init( &attr );
  cpu_set_t cpuset;

  for( unsigned int i = 0 ; i < g_num_threads ; i++ ) 
  {
      // run a thread on a core:
      CPU_ZERO( &cpuset );
      CPU_SET( i + 1, &cpuset );
      pthread_attr_setaffinity_np( &attr, sizeof( cpu_set_t ), &cpuset );
      pthread_create( &threads[i], &attr, thread_func, NULL );
  }

  int countdown = 20;

  while( g_num_threads_finished < g_num_threads && countdown-- ) 
  {
    std::this_thread::sleep_for( std::chrono::seconds( 1 ));
  }
  g_running = false;

  std::this_thread::sleep_for( std::chrono::seconds( 1 ));

  exit(0);
}
