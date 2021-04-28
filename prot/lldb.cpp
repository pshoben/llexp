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
    //variant_col_vec_t columns;
    int32_t time; // index of the time column in the time_cols vector
    int32_t price_i32; // index of the time column in the int32_cols vector
    int32_t qty_i64; // index of the qty column in the int64_cols vector

} table_t;
// can't construct my_table_t directly using shared_memory_manager : must wrap it in a shm-friendly container:
using tables_t = std::vector< table_t, std::scoped_allocator_adaptor< alloc< table_t >>>;

template< typename T >
void print_col( const T col ) 
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

template< typename T >
void print( const T cols ) 
{
  for( auto col : cols ) {
    print_col( col );
  }
}

template< typename T, typename U >
void populate_col( T & col, U x ) 
{
   col.resize(2);
   col[0][1]= x;
   col[1][1]=x;
}

void run_test ()
{
   bi::managed_shared_memory manager( bi::create_only, "Demo", 65536 );
 
   // create the column specializations:

//   variant_col_vec_t cols( manager.get_segment_manager());
//   variant_col_map_t col_map( manager.get_segment_manager());

//   int8_col_t int8_col(1);
   //col_map["table.sym"] = int8_col;

   int8_cols_t i8cols( manager.get_segment_manager());
   int16_cols_t i16cols( manager.get_segment_manager());
   int32_cols_t i32cols( manager.get_segment_manager());
   int64_cols_t i64cols( manager.get_segment_manager());
   float_cols_t  f32cols( manager.get_segment_manager());
   double_cols_t d64cols( manager.get_segment_manager());
   time_cols_t   t64cols( manager.get_segment_manager());

   map_string_int32_t colname_to_coltypes( manager.get_segment_manager());
   map_string_stringvec_t tablename_to_colnames( manager.get_segment_manager());

   my_tables_t   tabs( manager.get_segment_manager());

   tables_t tables( manager.get_segment_manager());
   table_t empty;
   tables.push_back(empty);

//   table_groups_t table_groups( manager.get_segment_manager());

   //my_tables_t tabs( manager.get_segment_manager());

   //my_table_t empty;
   //auto & tab = tabs.push_back( empty );
   for( int8_t i = 0 ; i < 2 ; ++i ) {
       int8_col_t & v = i8cols.emplace_back(2);
       populate_col( v , i );
   }
   for( int16_t i = 0 ; i < 2 ; ++i ) {
       int16_col_t & v = i16cols.emplace_back(2);
       populate_col( v , i );
   }
   for( int32_t i = 0 ; i < 2 ; ++i ) {
       int32_col_t & v = i32cols.emplace_back(2);
       populate_col( v , i );
   }
   for( int64_t i = 0 ; i < 2 ; ++i ) {
       int64_col_t & v = i64cols.emplace_back(2);
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
   print( i8cols );
   print( i16cols );
   print( i32cols );
   print( i64cols );

   bi::shared_memory_object::remove("Demo");
}

}

int main() {
  llexp::run_test();
}

