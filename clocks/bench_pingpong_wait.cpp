#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <atomic>
#include <cstdlib>
#include <x86intrin.h>
#include "common.h"
#include <cstring>
#include <iomanip>

using std::vector;
using std::mutex;

double g_nanos_per_cycle=0.0;

unsigned int g_num_threads=2;
uint64_t g_max_reads=500000000;

bool g_verbose = false;
bool g_do_align_to_cacheline = false;

typedef struct msg_t 
{
    uint64_t time;
    uint32_t counter;
} __attribute__((packed)) msg_t;

typedef struct thread_args_t {
    unsigned int this_thread_num;
    unsigned int write_to_thread_num;
} thread_args_t;

pthread_t threads[ 8 ];
thread_args_t g_thread_args[8];

std::size_t g_cacheline_size = 64;
std::size_t g_start_block_size = sizeof( msg_t );
std::size_t g_block_step = 13;
std::size_t g_block_size = 0; //  = g_start_block_size + (g_block_step * 4); // 64; // cache line size
char * g_thread_input_blocks; // array of block size, one per thread
int g_slot_size_pow2 = 6; // 2^6 = 64 ns slots

std::mutex g_mutex;

std::atomic<bool> g_running = true;
std::atomic<unsigned int> g_num_threads_finished = 0;

void take_mutex() { 
  g_mutex.lock(); 
}
void release_mutex() {
  g_mutex.unlock();
}

void * thread_measure_nanos( __attribute__((unused)) void * args ) 
{
    g_nanos_per_cycle = measure_nanos_per_cycle();
    return nullptr;
}

void * thread_func( __attribute__((unused)) void * args ) 
{
    auto option = "wait";
    unsigned int aux;

    // if this_thread_num is even, this is the 'starter thread' - it will write to its output location after a certain amount of time has passed
    // the other thread will wait until its input changes before acting

    thread_args_t thread_args = *((thread_args_t *) args);
    msg_t * input_msg = (msg_t *) &( g_thread_input_blocks [(thread_args.this_thread_num-1) * g_block_size]);
    msg_t * output_msg = (msg_t *) &( g_thread_input_blocks [(thread_args.write_to_thread_num-1) * g_block_size]);

    msg_t prev_input_msg { 0, 0 };
    msg_t prev_output_msg { 0, 0 };
     
    uint64_t num_reads = 1;
    uint64_t dropped = 0;
    int64_t sum_diff = 0; 
    int64_t sum_mean_diff = 0; 
    int64_t min_diff = INT64_MAX;
    int64_t max_diff = 0 ;
    int64_t running_avg = 0;
    int avg_countdown = 30000000;

    bool is_starter_thread = (thread_args.this_thread_num == 1);

    take_mutex();
    if( g_verbose )
    {
        pid_t tid = syscall( __NR_gettid );
        std::cout << " thread num " << thread_args.this_thread_num << " is starter thread? " << ((is_starter_thread)?"true":"false") << "\n";

        std::cout << option << " Thread ID " << tid << " on CPU " << sched_getcpu() << " - num " << thread_args.this_thread_num << " writing to " << thread_args.write_to_thread_num << "\n";
        printf("%s Thread ID %u on CPU %u input msg %p output msg %p separated by %ld bytes\n", option, tid, sched_getcpu(), (void*)input_msg, (void*)output_msg, (int64_t)output_msg-(int64_t)input_msg);
    }

    uint64_t time_now = __rdtscp(&aux);
    uint64_t prev_slot_num = time_now >> g_slot_size_pow2; 
    //auto prev_time = time_now;

    prev_output_msg.time = time_now;
    prev_output_msg.counter=1;
    prev_input_msg = prev_output_msg;

    *output_msg = prev_output_msg;
    //__builtin_prefetch( output_msg, 1/*for write*/,  0 /* no temporal locality*/ );

    release_mutex();

    msg_t current_input_msg;

    while( g_running && num_reads < g_max_reads ) 
    {
        // thread reads on a schedule:
        time_now = __rdtscp(&aux); 
        uint64_t slot_num = time_now >> g_slot_size_pow2;
        bool got_a_slot = slot_num & 1;

        if(( got_a_slot == is_starter_thread ) && (slot_num > prev_slot_num )) // 2 threads take alternate slots 
        {
            // next quantum
            prev_slot_num = slot_num;
//            if( g_verbose )
//            {
//                 take_mutex();
//                 std::cout << sched_getcpu() << " got slot " << slot_num <<  " at time " << time_now << "\n";
//                 release_mutex();
//            }
            //prev_time = time_now;

            // read shared location:
            current_input_msg.counter = input_msg->counter;
            current_input_msg.time = input_msg->time;
            //current_input_msg = *input_msg;
            // prefetch the next read while doing the post-read work:
            //__builtin_prefetch( input_msg, 0/*for read*/, 0  /* no temporal locality*/ );
 
//                if( g_verbose )
//                {
//                     take_mutex();
//                     std::cout << sched_getcpu() << " current input " << current_input_msg.counter <<  " prev in " << prev_input_msg.counter << "\n";
//                     release_mutex();
//                }
    
            if( current_input_msg.counter != prev_input_msg.counter ) 
            {
               // new input has been received
//                if( g_verbose )
//                {
//                     take_mutex();
//                     std::cout << sched_getcpu() << " new current input " << current_input_msg.counter <<  " prev in " << prev_input_msg.counter << "\n";
//                     release_mutex();
//                }
 
                num_reads++;

                if( current_input_msg.counter > ( prev_input_msg.counter+1 )) {
                    // some msgs have been dropped
                    dropped += ((prev_input_msg.counter - current_input_msg.counter) - 1);
                } 
   
                int64_t diff = current_input_msg.time - prev_input_msg.time;
                prev_input_msg = current_input_msg;
 
//                if( g_verbose )
//                {
//                    take_mutex();
//                    std::cout << sched_getcpu() << " READ : " << current_input_msg.counter << " " << diff << " from " << time_now << " - " << current_input_msg.time << "\n";
//                    release_mutex();
//                }
  
                if( diff < min_diff ) {
                     min_diff = diff;
                }   
                if( diff > max_diff ) {
                     max_diff = diff;
                }    
                sum_diff += diff;
                if( running_avg > 0) {
                    sum_mean_diff += llabs(diff - running_avg);
                }

           }
 
           // write a new timestamp on the output location:
    
           prev_output_msg.time = __rdtscp(&aux); 
           prev_output_msg.counter++;
           *output_msg = prev_output_msg;
           // prefetch the next write while doing the pre-write work:
           //__builtin_prefetch( output_msg, 1/*for write*/,  0 /* no temporal locality*/ );
           //_cldemote((void *)output_msg );
        
           avg_countdown--;
           // every so often, get the average (so far)
           if( avg_countdown == 0 ) {
    
               running_avg = sum_diff / num_reads;
               avg_countdown = 30000000;
   
               if( g_verbose )
               {
                   take_mutex();
                   //double drop_pct = (double) dropped / ((double) dropped + (double) num_reads) * 100.0;
                   std::cout << " | cpu " << sched_getcpu() << " | count " << num_reads << " | avg (cyc) " << (sum_diff / num_reads) << " | mad (cyc) " << (sum_mean_diff / num_reads) << "\n";
                   release_mutex();
               }
           } 
        }
    }

    take_mutex();
    if( g_verbose ) {
        std::cout << "Thread on CPU " << sched_getcpu() << " - read " << num_reads << " samples \n";
    }
    double avg_cyc = (double) sum_diff / (double) num_reads; 
    double mean_avg_diff = (double) sum_mean_diff / (double) num_reads; 
    //double drop_pct = (double) dropped / ((double) dropped + (double) num_reads) * 100.0;

    //std::cout << " | cpu  | step | blocksize  | count  | avg (cyc) | avg (ns) "  
    //<<  " | mad (cyc) | mad (ns) |\n";

    std::cout << std::fixed << std::setprecision(1);// << std::setw(10); 
    //std::cout << std::setw(10) << std::setfill(' ') ;

    std::cout << "| " << std::setw(9) << option;
  
    std::cout << " | " << sched_getcpu();
    std::cout << " | " << std::setw(10) << std::setfill(' ') << g_slot_size_pow2;
    std::cout << " | " << std::setw(10) << std::setfill(' ') << g_block_size;
    std::cout << " | " << std::setw(10) << std::setfill(' ') << num_reads;
    //std::cout << " | " << std::setw(10) << std::setfill(' ') << dropped;
    //std::cout << " | " << std::setw(10) << std::setfill(' ') << std::setprecision(2) << drop_pct;
    std::cout << " | " << std::setw(10) << std::setfill(' ') << avg_cyc;
    std::cout << " | " << std::setw(10) << std::setfill(' ') << avg_cyc * g_nanos_per_cycle;
    std::cout << " | " << std::setw(10) << std::setfill(' ') << mean_avg_diff;
    std::cout << " | " << std::setw(10) << std::setfill(' ') << mean_avg_diff * g_nanos_per_cycle;

    std::cout << " | " << std::setw(10) << std::setfill(' ') << min_diff;
    std::cout << " | " << std::setw(10) << std::setfill(' ') << min_diff * g_nanos_per_cycle;

    std::cout << " | " << std::setw(10) << std::setfill(' ') << max_diff;
    std::cout << " | " << std::setw(10) << std::setfill(' ') << max_diff * g_nanos_per_cycle;
 
    std::cout << "\n";

    release_mutex();

    g_num_threads_finished++;
    return nullptr;
}

int main(int argc, char * const * argv) 
{
  char c;
  while ((c = getopt (argc, argv, "avw:s:")) != -1) {
    switch (c)
    {
      case 'a':
        g_do_align_to_cacheline = true;
        break;
      case 'v':
        g_verbose = true;
        break;
      case 'w':
      {
        g_slot_size_pow2 = atoi(optarg);
        break;
      } 
      case 's':
      {
        g_block_step = atoi(optarg);
        g_block_size = g_start_block_size + (g_block_step * 4); // 64; // cache line size
        break;
      }
      default:
        printf("usage: (-a (=align)) (-v (=verbose)) (-s block-step) (-w slot size in pow2 nanos )\n");
        exit(0);
     }
  }

  pthread_attr_t attr;
  pthread_attr_init( &attr );
  cpu_set_t cpuset;

  {
      // run the clock measurement on core 1:
      CPU_ZERO( &cpuset );
      CPU_SET( 1, &cpuset );
      pthread_attr_setaffinity_np( &attr, sizeof( cpu_set_t ), &cpuset );
      pthread_t clock_thread;
      pthread_create( &clock_thread, &attr, thread_measure_nanos, NULL );
      pthread_join( clock_thread, 0 );
      // std::cout << "got " << option << " clock speed : " << g_nanos_per_cycle << " ns/cycle\n";
  }

  if( g_do_align_to_cacheline ) {
      g_block_size = g_cacheline_size;
  }
  if( g_block_size < 16) {
      g_block_size = 16;
  }

  // always align the first block
  size_t align_size = 64; // cacheline size
  double num_blocks = (double) g_num_threads * (double) g_block_size / (double) align_size;
  int rounded_blocks = (int) num_blocks + 1;
  size_t alloc_size = align_size * rounded_blocks;
  g_thread_input_blocks = (char *) aligned_alloc( align_size, alloc_size ); 
  if( g_verbose ) {
      printf("aligned_alloc( %lu, %u * %lu -> %lu rounded) returned %p\n", align_size, g_num_threads, g_block_size, alloc_size , g_thread_input_blocks );
  }

  memset( g_thread_input_blocks, 0, g_num_threads * g_block_size );
 
  for( unsigned int i = 0 ; i < g_num_threads ; i++ ) 
  {
      // run a thread on a core:
      CPU_ZERO( &cpuset );
      CPU_SET( i + 1, &cpuset );
      pthread_attr_setaffinity_np( &attr, sizeof( cpu_set_t ), &cpuset );

      g_thread_args[i].this_thread_num = i+1;
      g_thread_args[i].write_to_thread_num = (i+1)%g_num_threads + 1;

      take_mutex();
      pthread_create( &threads[i], &attr, thread_func, (void *)&( g_thread_args[i]));
      release_mutex();
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
