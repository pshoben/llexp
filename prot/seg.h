#ifndef LLEXP_PROT_SEG_H
#define LLEXP_PROT_SEG_H

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <iostream>

#define CACHELINE_SIZE 64
#define BLOCK_SIZE CACHELINE_SIZE * 65 // avoid 4K aliasing at start of block
#define NUM_BLOCKS    500 // 512*1000
#define SEGMENT_SIZE  NUM_BLOCKS * BLOCK_SIZE
#define STORAGE_SIZE  SEGMENT_SIZE

#define MAX_BLOCKS_PER_COLUMN 500 
#define MAX_COLUMNS 120

#define STORAGE_ID "/SHM_TEST"

using std::cout;

class Address // ptr address = segment-start-address + offset (max segment size supported = 4G)
{
public:
    uint32_t segment=0; // segment number of the address
		uint32_t offset=0;  // offset from start of segment of the start of the block
    bool operator==( Address const & other ) const {
        return ( segment == other.segment ) && ( offset == other.offset );
    }
    void print() {
        cout << " Address{ " << segment << ", " << offset << " }\n";
    }

}; // sizeof = 64 
constexpr Address NULL_ADDRESS{0,0};

// SegHeader contains the mapping of column names to columns, each of which must be in this segment ( TODO : may be in this segment or another )
class SegHeader 
{
public:
    uint32_t alloc_size=SEGMENT_SIZE; // the size of the segment in bytes
    uint32_t next_free_offset= BLOCK_SIZE; // the first unwritten byte - skip the first block used for this SegHeader
    char padding[ BLOCK_SIZE - ( sizeof( alloc_size )/* + sizeof( start_offset )*/ + sizeof( next_free_offset ))]; //  + sizeof( column_mappings ) + sizeof( num_columns ))];

    static SegHeader * instance()
    {
        static SegHeader * top = nullptr;
        static bool first_time = true; // first time instantiated, need to mmap it
        if( first_time ) {
            first_time = false;
          	// get shared memory file descriptor (NOT a file) - must already exist
          	int fd = shm_open( STORAGE_ID, O_RDWR, S_IRUSR | S_IWUSR );
          	if( fd == -1 )
          	{
          	  	perror("open");
          	}
            top = (SegHeader *) mmap( NULL, STORAGE_SIZE, PROT_WRITE, MAP_SHARED, fd, 0 );
	          if( top == MAP_FAILED )
	          {
		            perror("mmap");
		            top = nullptr;
	          }
        }
        return top;
    }
    // one writer thread
    // return Address{0,0} on fail
    Address alloc_block() {
        uint32_t alloc_offset = next_free_offset;
        if( next_free_offset + BLOCK_SIZE >= alloc_size ) {
            // out of space
            return NULL_ADDRESS;
        } else {
            __atomic_store_n( &next_free_offset, next_free_offset + BLOCK_SIZE, __ATOMIC_RELEASE );
            print();
            return Address{0,alloc_offset}; // just one segment
        }
    } 

    void print() 
    {
        cout << "alloc_size: " << alloc_size << "\n" 
             << "next_free_offset: " << next_free_offset << "\n"; 
    }
}; // sizeof = BLOCK_SIZE

// appendable is an array of arrays
class Appendable {
public:
    uint32_t current_address_index=0; // index into addresses array
    constexpr static size_t NUM_ADDRESSES = ( BLOCK_SIZE - sizeof( current_address_index )) / sizeof( Address ); 
    constexpr static size_t MAX_ADDRESS = NUM_ADDRESSES - 1;

    Address addresses[ NUM_ADDRESSES ];

    // single writer thread per Appendable
    bool append( char c ) 
    {
        SegHeader * top = SegHeader::instance(); // assert valid 
        top->print();
        if( addresses[ current_address_index ].offset >= BLOCK_SIZE )  { // [[ unlikely ]] 
            // current block full, take a new one
            if( current_address_index < ( MAX_ADDRESS )) { // [[ likely ]] 
                Address new_block = top->alloc_block();
                if( new_block == NULL_ADDRESS ) { // [[ unlikely ]] 
                    return false;
                }
                addresses[ ++current_address_index ] = new_block;
            } else {
                return false;
            }    
        } 
        Address top_address = addresses[ current_address_index ];   
        char * p = ((char*)top) + top_address.segment + top_address.offset;
        *p = c; 
        __atomic_store_n( &( addresses[ current_address_index ].offset ), top_address.offset + 1, __ATOMIC_RELEASE );
        return true;
    } 

    void print() {
        SegHeader * top = SegHeader::instance(); // assert valid 
        uint32_t current = __atomic_load_n( &( current_address_index ), __ATOMIC_ACQUIRE );
        for( uint32_t i=0 ; i <= current ; ++i ) {
            Address block_address = addresses[ i ];
            block_address.print();
            for( uint32_t j=0 ; j <= block_address.offset ; ++j ) {
                char * p =  ((char *)top) + block_address.segment + j ;
                cout << *p ;
            }
        }
        cout << "\n";
    }
}; // sizeof = BLOCK_SIZE

#endif
