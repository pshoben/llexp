#pragma once

#include <vector>
#include "common.h"
#include <climits>
#include <mutex>
#include <cmath>
#include <string>
#include <iomanip>

using std::string;

//#include <bit> c++20 / GCC9
// define BIT_WIDTH(x) std::bit_width(x)
#define BIT_WIDTH(X) ((unsigned) (8*sizeof (unsigned long long) - __builtin_clzll((X)) - 1)+1)

class ScaleRow 
{
public:
  // each int stores the raw count of values with exponent matching the index
  // e.g. value 0 is rounded up to 1 and goes in bucket 0 (2^0=1)  
  // e.g. value 1 is rounded up to 2 and goes in bucket 1 (2^1=2)
  // e.g. value 100 is rounded up to 128 and goes in bucket 7 (2^7=128) etc. 
  // it is a log (base2) scaled histogram
  // raw count values may be log(base2) scaled so that counts in the range 0-512 can be rendered as a single ascii space (or dot) for zero plus a single decimal digit in range 0-9 
  // and counts in the range 0-65536 can be represented by a single hex digit. larger counts can be represented by extending to base36 0-9..A-Z
  // 
  // e.g. this rendering: "0123456789ABCDEFGHIJKLMNOPQRSTUV"
  //                      "11     1 34                     "
  //
  // (representing the buckets : 0=1 1=1 2=0 3=0 4=0 5=0 6=0 7=1 8=0 9=3 10=4)
  // represents a log2 scale histogram rendering of these raw values : 0 1 100 312 500 510 520 800 830 1000 

  int samples_per_sec;

  unsigned int counts_per_sec[30][64];
  int scaled_per_sec[30][64];
  int row_count_per_sec[30];

  unsigned int counts[64];
  int scaled[64];
  int row_count=0;
  int max_column=-1;
  int max_column_per_sec=-1;

  time_t min_second=LONG_MAX;
 
  const char digits[63] = " 123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

  ScaleRow(int psamples_per_sec) : samples_per_sec( psamples_per_sec ) 
  {
    for( auto i = 0 ; i < 64 ; i++ ) 
      counts[i]=0;
    for( auto i = 0 ; i < 64 ; i++ ) 
      scaled[i]=-1;

    for( auto j = 0 ; j < 30 ; j++ ) 
      for( auto i = 0 ; i < 64 ; i++ ) 
        counts_per_sec[j][i]=0;

    for( auto j = 0 ; j < 30 ; j++ ) 
      for( auto i = 0 ; i < 64 ; i++ ) 
        scaled_per_sec[j][i]=-1;

    for( auto j = 0 ; j < 30 ; j++ ) 
      row_count_per_sec[j]=0;
  }
 
  void add_to_bucket_per_sec( unsigned int val, unsigned int current_sec ) 
  {
    unsigned int width = BIT_WIDTH(val);
    counts_per_sec[current_sec][width]++;
    row_count_per_sec[current_sec]++;
  }

  void scale_per_sec() 
  {
    for( auto j = 0 ; j < 30 ; j++ ){ 
      for( auto i = 0 ; i < 64 ; i++ ) {
        scaled_per_sec[j][i]= BIT_WIDTH( counts_per_sec[j][i] )-1; 
      }
    }
  }

  void calc_max_column_per_sec() 
  {
     for( auto j = 0 ; j < 30 ; j++ )
     { 
       // find max populated index for this second
       int i = 63;
       while( i>=0 && !counts[i] )
         i--;
       max_column_per_sec = std::max( max_column_per_sec, i ); // -1 means no columns populated
     }
  }

  std::string print_ruler_per_sec() 
  {
     calc_max_column_per_sec();
     std::string ret{""};
     if(max_column_per_sec >= 0) {
       for( auto i = 0 ; i <= max_column_per_sec ; i++ ) {
         ret.push_back(digits[i]); 
       }
    }
    return ret;
  }

  std::string print_rows_per_sec() 
  {
     std::string ret = print_ruler_per_sec() + "\n";
     char tmp[32];
     if( max_column_per_sec >= 0) {
       for( auto j = 0 ; j < 20 ; j++ )
       { 
         for( auto i = 0 ; i <= max_column_per_sec ; i++ ) {
           char * base62digit = to_string( tmp, scaled_per_sec[j][i], 36 );
           //std::cout << "..." << "[" << j << "][" << i << "] " <<  scaled_per_sec[j][i] << " " << counts_per_sec[j][i] << " = " << base62digit << "\n"; 
           ret.push_back(*base62digit); 
         }
         ret.push_back('\n');
       }
    }
    return ret;
  }


 
  void add_to_bucket( unsigned int val ) 
  {
    unsigned int width = BIT_WIDTH(val);
    counts[width]++;
    row_count++;
    //std::cout << " adding " << val << " to bucket " << width << "\n";
  }

  void scale_row() {
    for( auto i = 0 ; i < 64 ; i++ ) {
      scaled[i]= BIT_WIDTH( counts[i] )-1; 
    }
  }

  void calc_max_column() 
  {
     // find max populated index
     int i = 63;
     while( i>=0 && !counts[i] )
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


  std::string print_ruler() 
  {
     calc_max_column();
     std::string ret{""};
     if(max_column >= 0) {
       for( auto i = 0 ; i <= max_column ; i++ ) {
         ret.push_back(digits[i]); 
       }
    }
    return ret;
  }

  std::string print_row() 
  {
     calc_max_column();
     char tmp[32];
     std::string ret{""};
     if(max_column >= 0) {
       for( auto i = 0 ; i <= max_column ; i++ ) {
         //std::cout << "..." << scaled[i] << " " << counts[i] << "\n"; 
         char * base62digit = to_string( tmp, scaled[i], 36 );
         ret.push_back(*base62digit); 
       }
    }
    return ret;
  }

};

class MessageStats {
public:
   unsigned int num_batches;
   unsigned int max_writes;
   unsigned int num_samples_per_sec;
   unsigned int num_thread_pairs;
   std::vector< TimespecPair *> batches;
   std::mutex mutex;

   //ScaleRow * histogram_all;

   MessageStats( unsigned int pnum_batches, unsigned int pmax_writes, unsigned int pnum_samples_per_sec, unsigned int pnum_thread_pairs ) 
               : num_batches( pnum_batches ), 
                 max_writes( pmax_writes ), 
                 num_samples_per_sec( pnum_samples_per_sec ),
                 num_thread_pairs( pnum_thread_pairs )
                 //histogram_all( new ScaleRow( num_samples_per_sec )) 
   { }
   ~MessageStats()
   {
      for( auto owned : batches ) {
          delete[] (owned);
      }
      //delete histogram_all;
   }

   void process_batch( TimespecPair * batch, __attribute__((unused))bool first_batch ) 
   {
      //unsigned int ignore_first_n_entries = 1000 ;
      //std::cout << "processing a batch\n";
      unsigned int count=0;
      TimespecPair * p_batch_entry = batch;
      while( count < max_writes ) {
         long latency = timespec_diff_ns( p_batch_entry->write_time, p_batch_entry->read_time ); 
         //if( count <= ignore_first_n_entries ) {
         //   std::cout << "cold latency : " << latency << "\n";
         //} 
         latency_sum += latency;
         if( latency < latency_min) latency_min = latency;
         if( latency > latency_max) latency_max = latency;

//         if( p_batch_entry->write_time.tv_sec < histogram_all->min_second) { 
//            histogram_all->min_second = p_batch_entry->write_time.tv_sec;
//         } 
//         histogram_all->add_to_bucket((unsigned int)latency);

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
      //total_count += count;
   }

  void process_batch_per_sec( TimespecPair * batch ) 
   {
      TimespecPair * p_batch_entry = batch;
      unsigned int count=0;
      while( count < max_writes ) {
         long latency = timespec_diff_ns( p_batch_entry->write_time, p_batch_entry->read_time ); 
//         if( p_batch_entry->write_time.tv_sec - histogram_all->min_second < 30 ) {
//             histogram_all->add_to_bucket_per_sec((unsigned int)latency, (unsigned int) p_batch_entry->write_time.tv_sec - histogram_all->min_second );
//         }
         p_batch_entry++;
         count++;
      }
   }


   void print_stats() 
   {
       //std::cout << " got all " << batches.size() << " batches\n";
       bool first_batch = true;
       for( auto batch : batches ) {
           process_batch( batch, first_batch );
           first_batch = false;
       }
 
       std::cout << " threads " << std::setw(2) << std::setfill(' ') << num_thread_pairs*2 ; 
  
       std::cout << " rate " << std::setw(9) << std::setfill(' ') << num_samples_per_sec ;
       std::cout << " count " << std::setw(9) << std::setfill(' ') << total_count; //  << " sum latency  : " << latency_sum << "\n"; 
       //std::cout << " min " << std::setw(12) << std::setfill(' ') << latency_min;
       //std::cout << " max " << std::setw(12) << std::setfill(' ') << latency_max; 

       latency_avg = double(latency_sum) / double(total_count) ;

       std::cout << " avg " << std::setw(12) << std::setfill(' ') << latency_avg; 

       for( auto batch : batches ) {
           std_dev_batch( batch );
       }
       latency_std_dev = sqrt(double( double(sum_of_squares) / ( double(total_count) - 1.0)));
       double latency_2dev =  latency_std_dev * 2;
       double latency_3dev =  latency_std_dev * 3;

       std::cout << " dev " << std::setw(12) << std::setfill(' ') << latency_std_dev;
       std::cout << " sig " << std::setw(12) << std::setfill(' ') << double(latency_avg + latency_std_dev);
       std::cout << " 2sig " << std::setw(12) << std::setfill(' ') << double(latency_avg + latency_2dev);
       std::cout << " 3sig " << std::setw(12) << std::setfill(' ') << double(latency_avg + latency_3dev) << "\n";


//       std::cout << "\nHistogram column = latency (log2) ;  cell value  = Log2 count\n\n";
//       histogram_all->scale_row();
//       std::cout << histogram_all->print_ruler() << "\n";
//       std::cout << histogram_all->print_row() << "\n"; 

       // first pass has got min_second
//       for( auto batch : batches ) {
//           process_batch_per_sec( batch );
//       }
       //std::cout << "\nHistogram row = per second ; column = latency (log2) ;  cell value  = Log2 count\n\n";
       //std::cout << histogram_all->print_rows_per_sec() << "\n"; 

   }

   void add_batch( TimespecPair * batch ) 
   {
       std::lock_guard guard(mutex);
       batches.push_back(batch);
       if( batches.size() == num_batches ) {
          // got them all
          //std::cout << "got them all\n";
          print_stats();
       }
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

