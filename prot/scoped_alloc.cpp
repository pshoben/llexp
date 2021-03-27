#include <iostream>
#include <vector>
#include <scoped_allocator>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/adaptive_pool.hpp>

using std::cout;

namespace bi = boost::interprocess;
template<class T> using alloc = bi::adaptive_pool<T,
                                    bi::managed_shared_memory::segment_manager>;

using ipc_row = std::vector<int, alloc<int>>;
using ipc_matrix = std::vector<ipc_row, std::scoped_allocator_adaptor<alloc<ipc_row>>>;

void print( const ipc_matrix & matrix ) 
{
  for( auto row : matrix ) {
    cout << "(capacity=" << row.capacity() << ") ";
    for( auto val : row ) {
      cout << val << " ";
    }
    cout << "\n";
  }
  cout << "\n";
}

int main ()
{
   bi::managed_shared_memory s(bi::create_only, "Demo", 65536);
 
   // create vector of vectors in shared memory
   ipc_matrix v(s.get_segment_manager());
 
   // for all these additions, the inner vectors obtain their allocator arguments
   // from the outer vector's scoped_allocator_adaptor
   v.resize(1); v[0].push_back(1);
   v.emplace_back(2);
   std::vector<int> local_row = {1,2,3};
   local_row.reserve( 64 );
   v.emplace_back(local_row.begin(), local_row.end());

   //std::vector<int> empty_row(64);
   /* ipc_row & new_block = */ v.emplace_back( 0 ) ;
   ipc_row & new_block = v.back();

   new_block.reserve(64); // empty_row.begin(), empty_row.end());


   print( v );
 
   bi::shared_memory_object::remove("Demo");
}

