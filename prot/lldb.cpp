#include <iostream>
#include <vector>
#include <scoped_allocator>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/adaptive_pool.hpp>

using std::cout;

namespace bi = boost::interprocess;
template<class T> using alloc = bi::adaptive_pool<T,
                                    bi::managed_shared_memory::segment_manager>;

using int32_arr_t = std::array<int32_t,64>;

//using ipc_row = std::vector<int, alloc<int>>;

using int32_col_t = std::vector< int32_arr_t, std::scoped_allocator_adaptor< alloc< int32_arr_t >>>;

using int32_cols_t = std::vector< int32_col_t, std::scoped_allocator_adaptor< alloc< int32_col_t >>>;

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

int main ()
{
   bi::managed_shared_memory manager( bi::create_only, "Demo", 65536 );
 
   // create the columns
   int32_cols_t cols( manager.get_segment_manager());

   for( int32_t i = 0 ; i < 5 ; ++i ) {
       int32_col_t & v = cols.emplace_back(2);

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
   print( cols );

   bi::shared_memory_object::remove("Demo");
}

