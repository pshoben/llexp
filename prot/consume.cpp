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
	void *addr;
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
	addr = mmap(NULL, STORAGE_SIZE, PROT_READ, MAP_SHARED, fd, 0);
	if (addr == MAP_FAILED)
	{
		perror("mmap");
		return 30;
	}

  SegHeader * header = (SegHeader *)addr;
  header->print();

  char * start_addr = ((char*)addr); // + header->start_offset;
  //uint32_t alloc_size = header->alloc_size;

  // acquire/release semantics : when writer thread store/release next_free_offset, all writer writes that happened-before the store/release are visible here
  uint32_t prev_free_offset = __atomic_load_n( &( header->next_free_offset ), __ATOMIC_ACQUIRE );
  uint32_t num_spins = 0;
  do {
      //printf("prev was %u\n", prev_free_offset);
      // acquire/release semantics : when writer thread store/release next_free_offset, all writer writes that happened-before the store/release are visible here
      uint32_t next_free_offset = __atomic_load_n( &( header->next_free_offset ), __ATOMIC_ACQUIRE );
      //printf("got next  %u\n", next_free_offset);

      uint32_t num = next_free_offset - prev_free_offset;
      if( num > 0 ) {
          header->print();
          cout << "num spins " << num_spins << " prev_free_offset = " << prev_free_offset << " next is " << next_free_offset << "\n";
          num_spins=0;

//          size_t offset = header->start_offset;
//          //char * p = ((char *)start_addr)+start_offset;
//          //char * end = ((char *)start_addr)+next_free_offset;
//          do {
//             char * p = ((char *)start_addr)+offset;
//             cout << offset << " : [" << *p << "] ok\n"; 
//          } while ( offset++ < (next_free_offset-1) );
//          cout << "\n";
 
          while ( prev_free_offset < next_free_offset ) {
              // print deltas since last update
              char * p = start_addr+prev_free_offset-1;
              printf("%c %d (at %u addr %p) -(next %u addr = %p)\n",*p,*p, prev_free_offset, p, next_free_offset, start_addr+next_free_offset);
              cout << "[" << *p << "] "; 
              prev_free_offset++;
          } 
 
      } else {
          num_spins++;
      } 
     // busy spin
      std::this_thread::sleep_for( std::chrono::milliseconds( 490 ));

  } while( true ) ;

	// mmap cleanup happens on exit
//	res = munmap(addr, STORAGE_SIZE);
//	if (res == -1)
//	{
//		perror("munmap");
//		return 40;
//	}

//	// shm_open cleanup
//	fd = shm_unlink(STORAGE_ID);
//	if (fd == -1)
//	{
//		perror("unlink");
//		return 100;
//	}

  // never reaches:
	return 0;
}
