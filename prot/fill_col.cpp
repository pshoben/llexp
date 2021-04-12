#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <thread>
#include <chrono>

#include "seg.h"

#define STORAGE_ID "/SHM_TEST"
#define STORAGE_SIZE SEGMENT_SIZE
#define DATA "Hello, World! From PID %d"

int main() // int argc, char *argv[])
{
	int res;
	int fd;
	//int len;
	pid_t pid;
	void *addr;
	char data[STORAGE_SIZE];

	pid = getpid();
	sprintf(data, DATA, pid);

	// get shared memory file descriptor (NOT a file) - must already exist
	fd = shm_open(STORAGE_ID, O_RDWR, S_IRUSR | S_IWUSR);
	if (fd == -1)
	{
		perror("open");
		return 10;
	}

//	// extend shared memory object as by default it's initialized with size 0
//	res = ftruncate(fd, STORAGE_SIZE);
//	if (res == -1)
//	{
//		perror("ftruncate");
//		return 20;
//	}

	// map shared memory to process address space
	addr = mmap(NULL, STORAGE_SIZE, PROT_WRITE, MAP_SHARED, fd, 0);
	if (addr == MAP_FAILED)
	{
		perror("mmap");
		return 30;
	}

  TopSegment * header = (TopSegment *)addr;
  //SegHeader * header = (SegHeader *)addr;
  header->print();

  Column * col0 = header->goc_column( "test_col.0" );
  Column * col1 = header->goc_column( "test_col.1" );

  header->print();

  uint32_t i = 0;
  while( i < BLOCK_SIZE - 8 ) {
      col0->append( 'x' );
      i++;
      header->print();
      std::this_thread::sleep_for( std::chrono::seconds( 1 ));
  } 
//  char * pfill = ((char*)addr) + header->start_offset;
//  uint32_t alloc_size = header->alloc_size;
//  uint32_t next_free_offset = header->next_free_offset;
//  while( next_free_offset < alloc_size ) {
//      printf("writing byte at %p offset %lu\n",pfill, pfill - ((char*)addr)); 
//      *pfill++ = 'x'; // write a byte
//      // increment the next free offset
//      next_free_offset++;
//      // push the value to shared memory with write barrier. assumes this thread is the only writer.
//      // release semantics : readers that load/acquire next_free_offset are guaranteed to see all earlier writes from here
//      __atomic_store_n( &( header->next_free_offset ), next_free_offset, __ATOMIC_RELEASE );
//
//      header->print(); 
//      // wait for a bit
//      std::this_thread::sleep_for( std::chrono::seconds( 1 ));
//  } 

	// mmap cleanup
	res = munmap(addr, STORAGE_SIZE);
	if (res == -1)
	{
		perror("munmap");
		return 40;
	}

	return 0;
}
