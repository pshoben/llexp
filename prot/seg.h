#ifndef LLEXP_PROT_SEG_H
#define LLEXP_PROT_SEG_H

#include <stdint.h>
#include <iostream>

#define CACHELINE_SIZE 64
#define BLOCK_SIZE CACHELINE_SIZE * 65 // avoid 4K aliasing at start of block
#define NUM_BLOCKS     500 // 512*1000
#define SEGMENT_SIZE   NUM_BLOCKS * BLOCK_SIZE

#define MAX_BLOCKS_PER_COLUMN 500 

using std::cout;

class SegHeader {
public:
    uint32_t alloc_size;   // the size of the segment in bytes
    uint32_t start_offset; // the first byte to be written to
    uint32_t next_free_offset; // the first unwritten byte
    char pad[ CACHELINE_SIZE - 3*sizeof( uint32_t ) ];// pad to full line

    SegHeader() : alloc_size( SEGMENT_SIZE )
                , start_offset( sizeof( SegHeader ))
                , next_free_offset( sizeof( SegHeader ))
                , pad{0}
    {}
     
    void print() {
        auto start_addr = this;
        auto size = next_free_offset - start_offset;

        cout << "alloc_size: " << alloc_size << "\n" 
              << "start_offset: " << start_offset << "\n" 
              << "next_free_offset: " << next_free_offset << "\n" 
              << "start address = " << start_addr << "\n"
              << "capacity = " << alloc_size << "\n"
              << "size = " << size << "\n";

        size_t offset = start_offset;
        //char * p = ((char *)start_addr)+start_offset;
        //char * end = ((char *)start_addr)+next_free_offset;
        do {
           char * p = ((char *)start_addr)+offset;
           cout << *p;
           //cout << offset << " : [" << *p << "]\n"; 
        } while ( offset++ < (next_free_offset-1) );
        cout << "\n";
    }
} ;

class Column {
public:
    uint32_t top_block_offset; // offset of the top block from start of segmentthe most recently written block (index into start_of_block_offset array)
    uint32_t top_index; // the most recently written index in the top blocka
    uint32_t num_blocks; // used size of the start_of_block_offset array
    uint32_t start_of_block_offset[MAX_BLOCKS_PER_COLUMN];

 };

#endif
