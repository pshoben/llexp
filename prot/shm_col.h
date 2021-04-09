#include <cstdint>
#include <ctime>
#include <chrono>
#include <thread>
#include <cstdio>
#include <ctype.h>
#include <cstdlib>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <cstring>

#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); } while (0)

namespace lldb {

// global shm
class ControlBlock; // fwd declare


__extension__ typedef __int128 int128_t;
__extension__ typedef unsigned __int128 uint128_t;

constexpr uint32_t MAX_SEGMENTS = 62;
constexpr uint32_t SHM_CHUNK_SIZE = 64*1024; // 4K
constexpr uint32_t SHM_CHUNKS_PER_SEGMENT = 512;
constexpr uint32_t SHM_SEGMENT_SIZE = SHM_CHUNK_SIZE * SHM_CHUNKS_PER_SEGMENT ; // 2M

using ValueOffset = uint16_t; // offset of value from start of shm block
using BlockOffset = uint16_t; // offset of block from start of shm segement
using RegionOffset= uint16_t; // type of offset of region from start of shm segment
using SegmentId   = uint16_t; // type of unique shm segment identifier

typedef struct TimePair {
  time_t start_time;
  time_t end_time;
} TimePair;

typedef struct TimedSample1 {
  time_t start_time;
  int64_t latency;
} TimedSample1;

typedef struct TimedSample2 {
  time_t start_time;
  int32_t latency;
} TimedSample2;

typedef struct ValueAddress {
  SegmentId    segment;
  RegionOffset region;
  BlockOffset  block;
  ValueOffset  value;
} ValueAddress __attribute__((aligned (8))); // 64-bit value (atomically writable)

typedef struct RegionAddress {
  //SegmentId    segment;
  RegionOffset region;
} RegionAddress __attribute__((aligned (4))); // 64-bit value (atomically writable)

template< typename T>
class Block {
public:
  T values[ SHM_CHUNK_SIZE /sizeof(T)];
}; // a block is always 4 Kbytes in size

class Region {
public:
  BlockOffset blocks[ SHM_CHUNK_SIZE / sizeof( BlockOffset )];
}; // a region control struct is always 4Kb in size

typedef struct Chunk {
char bytes[SHM_CHUNK_SIZE];
} Chunk;

class ChunkLocation {
public:
  uint16_t segment_fd;
  uint16_t chunk_index; 
}; // absolute memory address = segment-start-address + chunk-index * chunk-size


constexpr int MAX_NAME_LEN = 64-sizeof(ValueAddress); // including null terminator
class Column {
public:
  ValueAddress top_value;
  char name[ MAX_NAME_LEN ]; // unique path to column, max len 63 chars
  ValueAddress allocated; // (value field ignored) ; avoid false sharing with top_value
  RegionAddress regions[( SHM_CHUNK_SIZE
                         -sizeof( top_value )
                         -sizeof( name )
                         -sizeof( allocated )) / sizeof( RegionAddress )];
}; // a column control struct is always 4Kb in size

typedef struct Chunk_KV {
uint32_t hash;
char name[ MAX_NAME_LEN ];
ChunkLocation location;
} Chunk_KV;  // key-value pair mapping a unique column name string to the chunk location in shared memory where the column header is stored


class Shm {
public:
  int fd_control=0;
  ControlBlock * control=nullptr;

  int new_segment( char * shmpath, /* out */Chunk ** new_chunk ) 
  {
    /* Create shared memory object and set its size to the size
       of our structure. */
printf("shm_open\n");
     int fd = shm_open(shmpath, O_CREAT /*| O_EXCL*/ | O_RDWR, S_IRUSR | S_IWUSR);
     if( fd == -1 )
        errExit("shm_open");

printf("ftruncate shm segment to size %u bytes\n", SHM_SEGMENT_SIZE);
    if( ftruncate( fd, SHM_SEGMENT_SIZE ) == -1 )
        errExit("ftruncate");

    /* Map the object into the caller's address space. */
printf("shm_mmap\n");
    *new_chunk = (Chunk *) mmap( NULL, SHM_SEGMENT_SIZE,
                               PROT_READ | PROT_WRITE,
                               MAP_SHARED, fd, 0 );
    if( *new_chunk == MAP_FAILED )
        errExit("mmap");
    return fd;
  }
 
  Shm(char * shmpath)  {
    fd_control = new_segment( shmpath, (Chunk**) &control ); 
  }
};
static Shm * g_shm=nullptr;

// the control block records the list of segments and maps column names to ChunkAddress of the Column header block inside a segment
// a column can cross multiple segments
typedef struct ControlBlock 
{
  constexpr static struct ChunkLocation null_ChunkLocation{0,0};

  int segment_fds[MAX_SEGMENTS];
  int next_lookup=0;
	ChunkLocation next_chunk_location;
  constexpr static int MAX_CHUNKS = ( SHM_CHUNK_SIZE
                         -sizeof( segment_fds )
                         -sizeof( next_lookup )
                         -sizeof( next_chunk_location )) / sizeof( Chunk_KV ) ; 

  Chunk_KV chunk_lookup[ MAX_CHUNKS ];

  // return true on success : fail if out of space
  bool increment( ChunkLocation & orig ) {
    ChunkLocation tmp = orig;
    if( tmp.chunk_index < SHM_CHUNKS_PER_SEGMENT ) {
      tmp.chunk_index++;
    } else {
      tmp.chunk_index = 0;
      
      int ret = g_shm->new_segment();
      if( ret < 0 ) { 
        return false;
      }
      tmp.segment_fd = ( uint16_t ) ret;
    }
    // only update on success
    orig = tmp;
    return true;
  }

  // get chunk if it exists, create it if it doesnt
  // return valid chunk location : success
  //        null chunk location : create fail 
  ChunkLocation gocChunk( char * name ) 
  {
    // TODO replace linear search
    for( int i = 0 ; i < MAX_CHUNKS ; ++i ) {
      if( !strcmp( name, chunk_lookup[i].name )) {
         return chunk_lookup[i].location ; 
      }
    }
    // not found - create it
    if( next_lookup == MAX_CHUNKS ) {
      return null_ChunkLocation;  // no more space - can't create 
    }
    Chunk_KV & next_chunk = chunk_lookup[ next_lookup++ ];
    strcpy( next_chunk.name, name );
    next_chunk.location = next_chunk_location;

    if( !increment( next_chunk_location )) {
      return null_ChunkLocation;  // create segment failed - can't create 
    } 
    return next_chunk.location; 
  }
} ControlBlock;



}

