#ifndef LLDB_H
#define LLDB_H

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <functional>
#include <utility>


using i32b_t = int32_t[64]; // a block is fixed alloc size, and populated as far as top_index
using i32_shcol_t = std::vector<i32b_t, ShmAlloc> // a column is a vector (i.e. growable) of blocks

typedef struct my_table_t // table with 2x int32 columns
{
  // TODO force alignment
  int32_t top_block; // this is a hazard index : only owning thread/process may increment
  int32_t top_index; // this is a hazard index : only owning thread/process may increment

  i32_shcol_t col1;
  i32_shcol_t col2;
} my_table_t;
using ShmTableAlloc = allocator<my_table_t, managed_shared_memory::segment_manager>;
using TableVecT = std::vector< my_table_t, ShmTableAlloc >;

#endif
