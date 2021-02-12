#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <cstdio>
#include <unistd.h>
#include "queue_wrapper.h"
#include <stdint.h>

using std::vector;
using std::mutex;


unsigned int g_num_thread_pairs=2;


void start_threads( unsigned int num_threads, vector<std::thread> & threads, const char * msg, int is_reader )
{
  QueueWrapper queue{};

  // A mutex ensures orderly access to std::cout from multiple threads.
  mutex iomutex;
  for (unsigned i = 0; i < num_threads; ++i) {

    if( is_reader ) {
      // reader thread lambda
      threads[i] = std::thread([&iomutex, i, msg, &queue] {
          while (1) {
            queue.read() ; 
            {
              // Use a lexical scope and lock_guard to safely lock the mutex only
              // for the duration of std::cout usage.
              std::lock_guard<std::mutex> iolock(iomutex);
              std::cout << "Thread #" << i << ": on CPU " << sched_getcpu() << " - " << msg << "\n";
            }
            // Simulate important work done by the tread by sleeping for a bit...
            std::this_thread::sleep_for(std::chrono::milliseconds(900));
          }
      });
    } else {
      // writer thread lambda
      threads[i] = std::thread([&iomutex, i, msg, &queue] {
        while (1) {
          queue.write() ; 
          {
            // Use a lexical scope and lock_guard to safely lock the mutex only
            // for the duration of std::cout usage.
            std::lock_guard<std::mutex> iolock(iomutex);
            std::cout << "Thread #" << i << ": on CPU " << sched_getcpu() << " - " << msg << "\n";
          }
          // Simulate important work done by the tread by sleeping for a bit...
          std::this_thread::sleep_for(std::chrono::milliseconds(900));
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
    if (rc != 0) {
      std::cerr << "Error calling pthread_setaffinity_np: " << rc << "\n";
    }
  }
}

int main(int argc, char * const * argv) 
{
  char c;
  while ((c = getopt (argc, argv, "n:")) != -1) {
    switch (c)
    {
      case 'n':
      {
        char * param_threads = optarg;
        g_num_thread_pairs = atoi(param_threads)/2;
        printf("got num threads %s = %d thread pairs\n", param_threads, g_num_thread_pairs); 
        break;
      }
      default:
        printf("usage affinity -n [num threads]\n");
        exit(0);
     }
  }

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

