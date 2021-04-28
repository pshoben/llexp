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

using std::cout;

int main() // int argc, char *argv[])
{
	//int res;
	int fd;
	//int len;
	pid_t pid;
	//void *addr;
	char data[STORAGE_SIZE];

	pid = getpid();
	sprintf(data, DATA, pid);

	// get shared memory file descriptor (NOT a file)
	fd = shm_open(STORAGE_ID, O_RDONLY, S_IRUSR | S_IWUSR);
	if (fd == -1)
	{
		perror("open");
		return 10;
	}

	// map shared memory to process address space
	void * addr = mmap(NULL, STORAGE_SIZE, PROT_READ, MAP_SHARED, fd, 0);
	if (addr == MAP_FAILED)
	{
		perror("mmap");
		return 30;
	}

  TopSegment * header = (TopSegment *)addr;
  header->print();
  Column * col0 = header->goc_column( "test_col.0" );

  char * start_addr = (char*)&(col0->data);

  uint32_t prev_free_index = __atomic_load_n( &( col0->next_free_index ), __ATOMIC_ACQUIRE );
  uint32_t num_spins = 0;
  do {
      uint32_t next_free_index = __atomic_load_n( &( col0->next_free_index ), __ATOMIC_ACQUIRE );
      //printf("got next  %u\n", next_free_offset);

      uint32_t num = next_free_index - prev_free_index;
      if( num > 0 ) {
          num_spins=0;
          while ( prev_free_index < next_free_index ) {
              // print deltas since last update
              char * p = start_addr + prev_free_index;
              //printf("%c %d (at %u addr %p) -(next %u addr = %p)\n",*p,*p, prev_free_offset, p, next_free_offset, start_addr+next_free_offset);
              cout << "[" << prev_free_index << "]: " << *p << "\n "; 
              prev_free_index++;
          } 
      } else {
          num_spins++;
      } 
      std::this_thread::sleep_for( std::chrono::milliseconds( 490 ));

  } while( true ) ;

  // never reaches:
	return 0;
}
