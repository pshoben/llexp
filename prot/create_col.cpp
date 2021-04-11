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
	pid_t pid;
	void *addr;
  //SegHeader * addr;
	char data[STORAGE_SIZE];

	pid = getpid();
	sprintf(data, DATA, pid);

	// get shared memory file descriptor (NOT a file)
	fd = shm_open(STORAGE_ID, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
	if (fd == -1)
	{
		perror("open");
		return 10;
	}

	// extend shared memory object as by default it's initialized with size 0
	res = ftruncate(fd, STORAGE_SIZE);
	if (res == -1)
	{
		perror("ftruncate");
		return 20;
	}

	// map shared memory to process address space
	addr =  mmap(NULL, STORAGE_SIZE, PROT_WRITE, MAP_SHARED, fd, 0);
	if (addr == MAP_FAILED)
	{
		perror("mmap");
		return 30;
	}

  // initialise the segment header
  //*addr = SegHeader{};

  // placement new the top segment

  TopSegment * header = new ((TopSegment *)addr) TopSegment();
  //SegHeader * header = (SegHeader *)addr;
  header->print();


  //SegHeader{};//  STORAGE_SIZE, 
                    //sizeof( SegHeader ),
                    //sizeof( SegHeader )};
 
  //addr->print(); 

////	// place data into memory
////	len = strlen(data) + 1;
////	memcpy(addr, data, len);
////
////	// wait for someone to read it
////	sleep(2);
//
//	// mmap cleanup
//	res = munmap(addr, STORAGE_SIZE);
//	if (res == -1)
//	{
//		perror("munmap");
//		return 40;
//	}
//
//	// shm_open cleanup
//	fd = shm_unlink(STORAGE_ID);
//	if (fd == -1)
//	{
//		perror("unlink");
//		return 100;
//	}

	return 0;
}

