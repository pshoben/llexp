#include <iostream>
#include <vector>
#include <scoped_allocator>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/adaptive_pool.hpp>

#include "lldb.h"

using std::cout;

namespace llexp {

typedef struct table_t
{
    hazard_t top; // written atomically
    int32_t time; // index of the time column in the time_cols vector
    int32_t price_i32; // index of the time column in the int32_cols vector
    int32_t qty_i64; // index of the qty column in the int64_cols vector

} table_t;
// can't construct my_table_t directly using shared_memory_manager : must wrap it in a shm-friendly container:
using tables_t = std::vector< table_t, std::scoped_allocator_adaptor< alloc< table_t >>>;

void print( const int32_col_t col ) 
{
  cout << "capacity : " << col.capacity() << "\n"; 
  cout << "size : " << col.size() << "\n"; 

  for( auto block : col ) {
    cout << "block : "; 
    for( auto val : block ) {
      cout << val << " ";
    }
    cout << "\n";
  }
  cout << "\n";
}

void print( const int32_cols_t cols ) 
{
  for( auto col : cols ) {
    print( col );
  }
}

void populate_col( int32_col_t & col, int32_t x ) 
{
   col.resize(2);
   col[0][0]=0;
   col[0][1]= x;

   col[1][0]=0;
   col[1][1]=x;
}

void run_test ()
{
   bi::managed_shared_memory manager( bi::create_only, "Demo", 65536 );
 
   // create the column specializations:
   int8_cols_t i8cols( manager.get_segment_manager());
   int16_cols_t i16cols( manager.get_segment_manager());
   int32_cols_t i32cols( manager.get_segment_manager());
   int64_cols_t i64cols( manager.get_segment_manager());

   float_cols_t  f32cols( manager.get_segment_manager());
   double_cols_t d64cols( manager.get_segment_manager());
   time_cols_t   t64cols( manager.get_segment_manager());

   my_tables_t   tabs( manager.get_segment_manager());

   tables_t tables( manager.get_segment_manager());
   table_t empty;
   tables.push_back(empty);

//   table_groups_t table_groups( manager.get_segment_manager());

   //my_tables_t tabs( manager.get_segment_manager());

   //my_table_t empty;
   //auto & tab = tabs.push_back( empty );

   for( int32_t i = 0 ; i < 5 ; ++i ) {
       int32_col_t & v = i32cols.emplace_back(2);

   //int32_col_t v( manager.get_segment_manager());
   //cols.push_back( v );
 
   // for all these additions, the inner vectors obtain their allocator arguments
   // from the outer vector's scoped_allocator_adaptor

       populate_col( v , i );
   }
 
//   v.resize(2);
//   v[0][0]=0;
//   v[0][1]=1;
//
//   v[1][0]=0;
//   v[1][1]=1;


//   v.resize(1); v[0].push_back(1);
//   v.emplace_back(2);
//   std::vector<int> local_row = {1,2,3};
//   local_row.reserve( 64 );
//   v.emplace_back(local_row.begin(), local_row.end());

   //std::vector<int> empty_row(64);
//   /* ipc_row & new_block = */ v.emplace_back( 0 ) ;
//   ipc_row & new_block = v.back();

//   new_block.reserve(64); // empty_row.begin(), empty_row.end());


   //print( v );
   print( i32cols );

   bi::shared_memory_object::remove("Demo");
}

}

int main() {
  llexp::run_test();
}

