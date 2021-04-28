#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "seg.h"

#define STORAGE_ID "/SHM_TEST"
#define STORAGE_SIZE  SEGMENT_SIZE
#define DATA "Hello, World! From PID %d"


int main() // int argc, char *argv[])
{
	int res;
	int fd;
	//int len;
	//pid_t pid;
	void * addr;
  SegHeader * header;
	//char data[STORAGE_SIZE];

	//pid = getpid();
	//sprintf(data, DATA, pid);

	// get shared memory file descriptor (NOT a file)
	fd = shm_open( STORAGE_ID, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR );
	if( fd == -1 )
	{
		perror("open");
		return 10;
	}

	// extend shared memory object as by default it's initialized with size 0
	res = ftruncate( fd, STORAGE_SIZE );
	if (res == -1)
	{
		perror("ftruncate");
		return 20;
	}

	// map shared memory to process headeress space
	addr = mmap( NULL, STORAGE_SIZE, PROT_WRITE, MAP_SHARED, fd, 0 );
	if( addr == MAP_FAILED )
	{
		perror("mmap");
		return 30;
	}

  // initialise the segment header
  header = new (addr) SegHeader{};
 
  header->print(); 

	return 0;
}

