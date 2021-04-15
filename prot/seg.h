#ifndef LLEXP_PROT_SEG_H
#define LLEXP_PROT_SEG_H

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

}; // sizeof = 64 
constexpr Address NULL_ADDRESS{0,0};

//class ColumnMapping 
//{
//public:
//    uint32_t col_id; // unique id of the column
//    uint32_t segment_number; // segment number of Column struct
//    uint32_t block_offset; // offset from start of segment of the start of the block
//    char name[20]; // /unique name of the column
//    void print( int i ) {
//        cout << "ColumnMapping[" << i << "] = { " << col_id << " , " << segment_number << " , " << block_offset << " , " << name << "}\n";
//    }
//}; // sizeof = 32

// SegHeader contains the mapping of column names to columns, each of which must be in this segment ( TODO : may be in this segment or another )
class SegHeader 
{
public:
    uint32_t alloc_size=SEGMENT_SIZE; // the size of the segment in bytes
    //uint32_t start_offset=0; // the first byte to be written to
    uint32_t next_free_offset= BLOCK_SIZE; // the first unwritten byte - skip the first block used for this SegHeader
    //uint32_t num_columns=0; // size of columns array

    //ColumnMapping column_mappings[MAX_COLUMNS];
    // pad to full block
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
//    // one writer thread
//    // return UINT32_MAX on fail 
//    uint32_t alloc_block() {
//        uint32_t alloc_offset = next_free_offset;
//        if( next_free_offset + BLOCK_SIZE >= alloc_size ) {
//            // out of space
//            return UINT32_MAX;
//        } else {
//          __atomic_store_n( &next_free_offset, next_free_offset + BLOCK_SIZE, __ATOMIC_RELEASE );
//          return alloc_offset;
//        }
//    } 
    // one writer thread
    // return Address{0,0} on fail
    Address alloc_block() {
        uint32_t alloc_offset = next_free_offset;
        if( next_free_offset + BLOCK_SIZE >= alloc_size ) {
            // out of space
            return NULL_ADDRESS;
        } else {
            __atomic_store_n( &next_free_offset, next_free_offset + BLOCK_SIZE, __ATOMIC_RELEASE );
            return Address{0,alloc_offset}; // just one segment
        }
    } 

    void print() 
    {
        //auto start_addr = this;
        //auto size = next_free_offset - start_offset;
        cout << "alloc_size: " << alloc_size << "\n" 
             //<< "start_offset: " << start_offset << "\n" 
             << "next_free_offset: " << next_free_offset << "\n"; 
             //<< "num_columns = " << num_columns << "\n";
//        for( uint32_t i = 0 ; i < num_columns ; ++i ) {
//            column_mappings[i].print(i);
//            Column * col = get_column( i ) ;
//            col->print();
//        }
    }

//    // get the column by index
//    Column * get_column( uint32_t i ) 
//    {
//        // if the named column exists, return its address
//        // if it doesn't exist, return null pointer
//        if( i < num_columns ) {
//             return (Column *)(char *)(((char*)this)+( column_mappings[i].block_offset ));
//        }
//        return nullptr;
//    }
//
//    // get or create the named column
//    Column * goc_column( const char * name ) 
//    {
//        // if the named column exists, return its address
//        // if it doesn't exist, create it and return its address
//        // if it can't be created, due to out of space, return null pointer
//        uint32_t i = 0 ;
//        while( i < num_columns ) {
//            if( !strcmp( name, column_mappings[i].name )) {
//                // found the column, return its address:
//                return (Column *)(char *)(((char*)this)+( column_mappings[i].block_offset ));
//            }
//            i++;
//        }
//        if( i == MAX_COLUMNS ) {
//            return nullptr;
//        }
//        // allocate the next free block in the segment
//        uint32_t new_block_offset = alloc_block();
//        if( new_block_offset == UINT32_MAX ) {
//            // out of space
//            return nullptr;
//        }
//        
//        // create the column
//        column_mappings[i]= ColumnMapping{ i, 0, new_block_offset , '\0' };
//        strcpy( column_mappings[i].name, name );
//        num_columns++;
//        // placement new construct the column
//        Column * addr = new ( ((char*)this ) + new_block_offset ) Column();
//        //addr->print( i, addr );
//        return addr;
//    }
     
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
            for( uint32_t j=0 ; j <= block_address.offset ; ++j ) {
                char * p =  ((char *)top) + block_address.segment + j ;
                cout << *p ;
            }
        }
        cout << "\n";
    }
}; // sizeof = BLOCK_SIZE



//// temp : column is just a block
//class Column {
//public:
//    uint32_t tag=0; // type info of the column
//    uint32_t next_free_index=0; // the size of the array (in units of width)
//    char data[ BLOCK_SIZE - ( sizeof( tag ) + sizeof( next_free_index ))];
//
//    void print( int i, Column * addr ) {
//        cout << "Column[" << i << "] @" << (long)addr << " = { " << tag << " , " << next_free_index << "}\n";
//    }
//
//    void append( char c ) {
//        data[ next_free_index ] = c ;
//        // single writer thread owns the column
//        uint32_t next = next_free_index+1;
//        __atomic_store_n( &next_free_index, next, __ATOMIC_RELEASE );
//    } 
//
//    void print() {
//        cout << "tag: " << tag << "\n" 
//             << "next_free_index: " << next_free_index << "\n"; 
//        for( uint32_t i=0 ; i < next_free_index ; ++i ) {
//            cout << data[i];
//        }
//        cout << "\n";
//    }
//}; // sizeof = BLOCK_SIZE

//class SegHeader {
//public:
//    uint32_t alloc_size;   // the size of the segment in bytes
//    uint32_t next_free_offset; // the first unwritten byte
//    char pad[ CACHELINE_SIZE - 3*sizeof( uint32_t ) ];// pad to full line
//
//    SegHeader() : alloc_size( SEGMENT_SIZE )
//                //, start_offset( sizeof( SegHeader ))
//                , next_free_offset( sizeof( SegHeader ))
//                , pad{0}
//    {}
//  
//    void print() {
//        // print might be called from a reader thread
//        uint32_t prev_free_offset = __atomic_load_n( &( next_free_offset ), __ATOMIC_ACQUIRE );
//        cout << "alloc_size: " << alloc_size << "\n" 
//             << "next_free_offset: " << prev_free_offset << "\n" 
//             << "start address = " << start_addr << "\n";
//        size_t offset = prev_start_offset;
//        do {
//           char * p = ((char *)this)+offset;
//           cout << *p;
//        } while ( offset++ < ( prev_free_offset ));
//        cout << "\n";
//    }
//} ;

#endif
