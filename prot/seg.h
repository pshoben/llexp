#ifndef LLEXP_PROT_SEG_H
#define LLEXP_PROT_SEG_H

#include <stdint.h>
#include <iostream>

#define CACHELINE_SIZE 64
#define BLOCK_SIZE CACHELINE_SIZE * 65 // avoid 4K aliasing at start of block
#define NUM_BLOCKS     500 // 512*1000
#define SEGMENT_SIZE   NUM_BLOCKS * BLOCK_SIZE

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

        char * p = ((char *)start_addr)+start_offset;
        char * end = ((char *)start_addr)+next_free_offset;
        do {
           cout << *p; 
        } while ( p++ < end );
        cout << "\n";
    }
} ;

#endif
