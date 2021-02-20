#pragma once

#include <vector>
#include "common.h"
#include <climits>
#include <mutex>
#include <cmath>
#include <string>
#include <iomanip>
#include <algorithm>

using std::string;

//#include <bit> c++20 / GCC9
// define BIT_WIDTH(x) std::bit_width(x)
#define BIT_WIDTH(X) ((unsigned) (8*sizeof (unsigned long long) - __builtin_clzll((X)) - 1))

class ScaleRow 
{
public:
  // each int stores the raw count of values with exponent matching the index
  // e.g. value 0 is rounded up to 1 and goes in bucket 0 (2^0=1)  
  // e.g. value 1 is rounded up to 2 and goes in bucket 1 (2^1=2)
  // e.g. value 100 is rounded up to 128 and goes in bucket 7 (2^7=128) etc. 
  // it is a log (base2) scaled histogram
  // raw count values may be log(base2) scaled so that counts in the range 0-512 can be rendered as a single ascii dot for zero plus a single decimal digit in range 0-9 
  // and counts in the range 0-65536 can be represented by a single hex digit. larger counts can be represented by extending to base36 0-9..A-Z
  // 
  // e.g. this rendering: "0123456789ABCDEFGHIJKLMNOPQRSTUV"
  //                      "11     1 34                     "
  //
  // (representing the buckets : 0=1 1=1 2=0 3=0 4=0 5=0 6=0 7=1 8=0 9=3 10=4)
  // represents a log2 scale histogram rendering of these raw values : 0 1 100 312 500 510 520 800 830 1000 

  int samples_per_sec;

  unsigned int counts[64];
  int scaled[64];
  int row_count=0;
  int max_column=-1;

  time_t min_second=LONG_MAX;
 
  const char digits[63] = ".123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

  ScaleRow(int psamples_per_sec) : samples_per_sec( psamples_per_sec ) 
  {
    for( auto i = 0 ; i < 64 ; i++ ) 
      counts[i]=0;
    for( auto i = 0 ; i < 64 ; i++ ) 
      scaled[i]=-1;
  }
 
  void add_to_bucket( unsigned int val ) 
  {
    unsigned int width = BIT_WIDTH(val);
    counts[width]++;
    row_count++;
    //std::cout << " adding " << val << " to bucket " << width << " giving count " << counts[width] << " \n";
  }

  void scale_row() 
  {
    for( auto i = 0 ; i < 64 ; i++ ) { 
      unsigned int count = counts[i];
      
      if( count > 0) {
        scaled[i]= BIT_WIDTH( count )+1;
      }
      //std::cout << " scaled[" << i << "] = " << scaled[i] << " count = " << count << " \n";
    }
  }

  void calc_max_column() 
  {
     // find max populated index
     int i = 63;
     while( i >= 0 && counts[i] <= 0 )
       i--;
     max_column = i; // -1 means no columns populated
  }

  char *reverse(char *str)
  {
     /*https://stackoverflow.com/questions/59751447/convert-long-integerdecimal-to-base-36-string-strtol-inverted-function-in-c*/
      char *end = str;
      char *start = str;
      if(!str || !*str) return str;
      while(*(end + 1)) end++;
      while(end > start)
      {
          int ch = *end;
          *end-- = *start;
          *start++ = ch;
      }
      return str;
  }

  char *to_string(char *buff, long long num, unsigned long base)
  {
     if (num < 0)
       return (char *)digits; // point to 0 element ' '

     /*https://stackoverflow.com/questions/59751447/convert-long-integerdecimal-to-base-36-string-strtol-inverted-function-in-c*/
     int sign = num < 0;
      char *savedbuff = buff;
      if(base < 2 || base >= sizeof(digits)) return NULL;
      if(buff) {
          do {
              *buff++ = digits[abs(num % base)];
              num /= base;
          }while(num);
          if(sign) {
              *buff++ = '-';
          }
          *buff = 0;
          reverse(savedbuff);
      }
      return savedbuff;
  }

//  std::string print_ruler() 
//  {
//     calc_max_column();
//     std::string ret{""};
//     if(max_column >= 0) {
//       for( auto i = 0 ; i <= max_column ; i++ ) {
//         ret.push_back(digits[i]); 
//       }
//    }
//    return ret;
//  }

  std::string print_row() 
  {
     calc_max_column();
     char tmp[66];
     std::string ret{""};
     if(max_column >= 0) {
       for( auto i = 0 ; i <= max_column ; i++ ) {
         char * base62digit = to_string( tmp, scaled[i], 62 );
         //std::cout << "i = " << i << " max_col = " << max_column << "..." << scaled[i] << " " << counts[i] << " -> " << *base62digit << "\n"; 
         ret.push_back(*base62digit); 
       }
    }
    return ret;
  }

};

class MessageStats {
public:
   string name;
   unsigned int max_writes;
   unsigned int num_samples_per_sec;
   unsigned int num_thread_pairs;
   std::vector< TimespecPair *> batches;
   std::mutex mutex;
   std::vector<unsigned int>all_latencies;

   ScaleRow * histogram_all;

   MessageStats( string pname, unsigned int pmax_writes, unsigned int pnum_samples_per_sec, unsigned int pnum_thread_pairs ) 
               : name( pname ),
                 max_writes( pmax_writes ), 
                 num_samples_per_sec( pnum_samples_per_sec ),
                 num_thread_pairs( pnum_thread_pairs ),
                 histogram_all( new ScaleRow( num_samples_per_sec )) 
   {
       all_latencies.reserve( pmax_writes * pnum_thread_pairs );
   }
   ~MessageStats()
   {
      for( auto owned : batches ) {
          delete[] (owned);
      }
      delete histogram_all;
   }

   void process_batch( TimespecPair * batch, __attribute__((unused))bool first_batch ) 
   {
      unsigned int count=0;
      TimespecPair * p_batch_entry = batch;
      while( count < max_writes ) {
         long latency = timespec_diff_ns( p_batch_entry->write_time, p_batch_entry->read_time ); 
         latency_sum += latency;
         if( latency < latency_min) latency_min = latency;
         if( latency > latency_max) latency_max = latency;

         all_latencies.push_back((unsigned int)latency);
         histogram_all->add_to_bucket((unsigned int)latency);

         p_batch_entry++;
         count++;
      }
      total_count += count;
     //std::cout << " count " << total_count << " sum latency at batch : " << latency_sum << "\n"; 
   }

   void std_dev_batch( TimespecPair * batch ) 
   {
      unsigned int count=0;
      TimespecPair * p_batch_entry = batch;
      while( count < max_writes ) {
         long latency = timespec_diff_ns( p_batch_entry->write_time, p_batch_entry->read_time ); 
         long std_diff = latency - latency_avg;
         sum_of_squares += std_diff * std_diff;
         p_batch_entry++;
         count++;
      }
   }

   void print_stats() 
   {
       bool first_batch = true;
       for( auto batch : batches ) {
           process_batch( batch, first_batch );
           first_batch = false;
       }

       // calculate percentiles

       std::sort( all_latencies.begin(), all_latencies.end());

       auto latency_50th_pct = all_latencies[ all_latencies.size() * 50 / 100 ];
       auto latency_90th_pct = all_latencies[ all_latencies.size() * 90 / 100 ];
       auto latency_95th_pct = all_latencies[ all_latencies.size() * 95 / 100 ];
       auto latency_99th_pct = all_latencies[ all_latencies.size() * 99 / 100 ];
       auto latency_99_5th_pct = all_latencies[ (all_latencies.size() * 995.0) / 1000.0 ];
       auto latency_99_7th_pct = all_latencies[ (all_latencies.size() * 997.0) / 1000.0 ];

       auto latency_100th_pct = all_latencies[ all_latencies.size()-1 ] ;
      
       std::cout << "| " << std::setw(4) << std::setfill(' ') << name ; 
 
       std::cout << " | " << std::setw(4) << std::setfill(' ') << num_thread_pairs*2 ; 
  
       std::cout << " | " << std::setw(14) << std::setfill(' ') << num_samples_per_sec ;
       std::cout << " | " << std::setw(15) << std::setfill(' ') << total_count; //  << " sum latency  : " << latency_sum << "\n"; 

       std::cout << " | " << std::setw(10) << std::setfill(' ') << latency_50th_pct ; 
       std::cout << " | " << std::setw(10) << std::setfill(' ') << latency_90th_pct ; 
       std::cout << " | " << std::setw(10) << std::setfill(' ') << latency_95th_pct ; 
       std::cout << " | " << std::setw(10) << std::setfill(' ') << latency_99th_pct ; 
       std::cout << " | " << std::setw(10) << std::setfill(' ') << latency_99_5th_pct ; 
       std::cout << " | " << std::setw(10) << std::setfill(' ') << latency_99_7th_pct ; 
       std::cout << " | " << std::setw(10) << std::setfill(' ') << latency_100th_pct ; 

// avg / stddev stuff

//       latency_avg = double(latency_sum) / double(total_count) ;
//       if( std::isnan(latency_avg))
//         std::cout << " | " << std::setw(16) << std::setfill(' ') << latency_avg;
//       else 
//         std::cout << " | " << std::setw(16) << std::setfill(' ') << long(latency_avg);
//
//
//       for( auto batch : batches ) {
//           std_dev_batch( batch );
//       }
//       latency_std_dev = sqrt(double( double(sum_of_squares) / ( double(total_count) - 1.0)));
//       double latency_2dev =  latency_std_dev * 2;
//       double latency_3dev =  latency_std_dev * 3;
//
//       if( std::isnan(latency_std_dev))
//         std::cout << " | " << std::setw(16) << std::setfill(' ') << latency_std_dev;
//       else 
//         std::cout << " | " << std::setw(16) << std::setfill(' ') << long(latency_std_dev);
//
//       double sigma = double(latency_avg + latency_std_dev); // 68% of all samples are less than sigma
//       if( std::isnan( sigma ))
//         std::cout << " | " << std::setw(16) << std::setfill(' ') << sigma;
//       else
//         std::cout << " | " << std::setw(16) << std::setfill(' ') << long(sigma);
//
//       double sigma2 = double(latency_avg + latency_2dev); // 95% of all samples are less than 2 sigmas
//       if( std::isnan( sigma ))
//         std::cout << " | " << std::setw(17) << std::setfill(' ') << sigma2;
//       else
//         std::cout << " | " << std::setw(17) << std::setfill(' ') << long(sigma2);
//
//       double sigma3 = double(latency_avg + latency_3dev); // 99.7% of all samples are less than 3 sigmas
//       if( std::isnan( sigma ))
//         std::cout << " | " << std::setw(17) << std::setfill(' ') << sigma3;
//       else
//         std::cout << " | " << std::setw(17) << std::setfill(' ') << long(sigma3);

       histogram_all->scale_row();
       std::cout << " | " << histogram_all->print_row() << "\n"; 
   }

   void add_batch( TimespecPair * batch ) 
   {
       batches.push_back(batch);
   }

 private:
   long latency_sum=0;
   long total_count=0;
   long latency_min=LONG_MAX;
   long latency_max=LONG_MIN;
   double latency_avg=0;
   long sum_of_squares=0;
   double latency_std_dev=0;
};

