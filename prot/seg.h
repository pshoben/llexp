#ifndef LLEXP_PROT_SEG_H
#define LLEXP_PROT_SEG_H

#include <stdint.h>
#include <iostream>

#define CACHELINE_SIZE 64
#define BLOCK_SIZE CACHELINE_SIZE * 65 // avoid 4K aliasing at start of block
#define NUM_BLOCKS     500 // 512*1000
#define SEGMENT_SIZE   NUM_BLOCKS * BLOCK_SIZE

#define MAX_BLOCKS_PER_COLUMN 500 
#define MAX_COLUMNS 120

using std::cout;

class ColumnMapping 
{
public:
    uint32_t col_id; // unique id of the column
    uint32_t segment_number; // segment number of Column struct
    uint32_t block_number; // block number of Column struct
    char name[20]; // /unique name of the column

    void print( int i ) {
        cout << "ColumnMapping[" << i << "] = { " << col_id << " , " << segment_number << " , " << block_number << " , " << name << "}\n";
    }
}; // sizeof = 32

// temp : column is just a block
class Column {
public:
    uint32_t tag=0; // type info of the column
    uint32_t next_free_index=0; // the size of the array (in units of width)
    char data[ BLOCK_SIZE - ( sizeof( tag ) + sizeof( next_free_index ))];

    void print( int i, Column * addr ) {
        cout << "Column[" << i << "] @" << (long)addr << " = { " << tag << " , " << next_free_index << "}\n";
    }

}; // sizeof = BLOCK_SIZE


// TopSegment contains the mapping of column names to columns, each of which must be in this segment ( TODO : may be in this segment or another )
class TopSegment 
{
public:
    uint32_t alloc_size=SEGMENT_SIZE;   // the size of the segment in bytes
    uint32_t start_offset=0; // the first byte to be written to
    uint32_t next_free_offset= BLOCK_SIZE; // the first unwritten byte - skip the first block used for this TopSegment
    uint32_t num_columns=0; // size of columns array

    ColumnMapping columns[MAX_COLUMNS];
    // pad to full block
    char padding[ BLOCK_SIZE - ( sizeof( alloc_size ) + sizeof( start_offset ) + sizeof( next_free_offset ) + sizeof( columns ) + sizeof( num_columns ))];

    // only one writer thread has write access to segement so no synchronization needed
    // return UINT32_MAX on fail 
    uint32_t alloc_block() {
        uint32_t alloc_offset = next_free_offset;
        if( next_free_offset + BLOCK_SIZE >= alloc_size ) {
            // out of space
            return UINT32_MAX;
        } else {
          __atomic_store_n( &next_free_offset, next_free_offset + BLOCK_SIZE, __ATOMIC_RELEASE );
          return alloc_offset;
        }
    } 

    void print() {
        //auto start_addr = this;
        //auto size = next_free_offset - start_offset;

        cout << "alloc_size: " << alloc_size << "\n" 
             << "start_offset: " << start_offset << "\n" 
             << "next_free_offset: " << next_free_offset << "\n" 
             << "num_columns = " << num_columns << "\n";
        for( uint32_t i=0 ; i < num_columns ; ++i ) {
            columns[i].print(i);
        }
 
    }

    // get or create the named column
    Column * goc_column( const char * name ) 
    {
        // if the named column exists, return its address
        // if it doesn't exist, create it and return its address
        // if it can't be created, due to out of space, return null pointer
        uint32_t i = 0 ;
        while( i < num_columns ) {
            if( !strcmp( name, columns[i].name )) {
                // found the column, return its address:
                return (Column *) ((char*)this)+( BLOCK_SIZE * columns[i].block_number );
            }
            i++;
        }
        if( i == MAX_COLUMNS ) {
            return nullptr;
        }
        // allocate the next free block in the segment
        uint32_t new_block_offset = alloc_block();
        if( new_block_offset == UINT32_MAX ) {
            // out of space
            return nullptr;
        }
        
        // create the column
        columns[i]= ColumnMapping{ i, 0, new_block_offset , '\0' };
        strcpy( columns[i].name, name );
        num_columns++;
        // placement new construct the column
        Column * addr = new ( ((char*)this ) + new_block_offset ) Column();
        //addr->print( i, addr );
        return addr;
    }
     
}; // sizeof = BLOCK_SIZE

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

#endif
