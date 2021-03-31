#ifndef LLDB_H
#define LLDB_H

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <functional>
#include <utility>
#include <variant>

namespace bi = boost::interprocess;

namespace llexp {

template<class T> 
using alloc = bi::adaptive_pool<T, bi::managed_shared_memory::segment_manager>;

template<typename T, int Size=64>
using block_t = std::array<T,Size>;

template<typename T>
using col_t = std::vector< block_t<T>, std::scoped_allocator_adaptor< alloc< block_t< T >>>>;

//template<typename T>
//using cols_t = std::vector< col_t<T>, std::scoped_allocator_adaptor< alloc< col_t< T >>>>;

using int8_col_t = col_t<int8_t>;
using int16_col_t = col_t<int16_t>;
using int32_col_t = col_t<int32_t>;
using int64_col_t = col_t<int64_t>;
using float_col_t = col_t<float_t>;
using double_col_t = col_t<double_t>;
using time_col_t = col_t<time_t>;

using stringvec_t = std::vector< std::string, std::scoped_allocator_adaptor< alloc< std::string >>>;

using map_string_int32_t = std::map< std::string, int32_t, std::scoped_allocator_adaptor< alloc< int32_t >>>;
using map_string_stringvec_t = std::map< std::string, stringvec_t, std::scoped_allocator_adaptor< alloc< stringvec_t >>>;

// need a shm aware stl container to store each specialised column type
using int8_cols_t = std::vector< int8_col_t, std::scoped_allocator_adaptor< alloc< int8_col_t >>>;
using int16_cols_t = std::vector< int16_col_t, std::scoped_allocator_adaptor< alloc< int16_col_t >>>;
using int32_cols_t = std::vector< int32_col_t, std::scoped_allocator_adaptor< alloc< int32_col_t >>>;
using int64_cols_t = std::vector< int64_col_t, std::scoped_allocator_adaptor< alloc< int64_col_t >>>;
using float_cols_t = std::vector< float_col_t, std::scoped_allocator_adaptor< alloc< float_col_t >>>;
using double_cols_t = std::vector< double_col_t, std::scoped_allocator_adaptor< alloc< double_col_t >>>;
using time_cols_t = std::map< std::string, time_col_t, std::scoped_allocator_adaptor< alloc< time_col_t >>>;


//using variant_col_t = std::variant< int8_col_t, int16_col_t >;
//using variant_col_vec_t = std::vector< variant_col_t, std::scoped_allocator_adaptor< alloc< variant_col_t >>>;
//using variant_col_map_t = std::map< std::string, variant_col_t, std::scoped_allocator_adaptor< alloc< variant_col_t >>>;

//
//using variant_cols_t = std::variant<int8_cols_t, int16_cols_t >;
//using variant_cols_vec_t = std::vector< variant_cols_t, std::scoped_allocator_adaptor< alloc< variant_cols_t >>>;



typedef struct hazard_t
{
  int32_t top_block; // the index of the block currently being written to
  int32_t top_index; // the last written index of the top block
} hazard_t;


typedef struct my_table_t
{
    hazard_t top; // written atomically

    int32_t time;      // index in time vector
    int32_t latency_i32; // index in latency int32 cols vector

} my_table_t;
// can't construct my_table_t directly using shared_memory_manager : must wrap it in a shm-friendly container:
using my_tables_t = std::vector< my_table_t, std::scoped_allocator_adaptor< alloc< my_table_t >>>;

}

#endif
